#!/usr/bin/env python3
"""
Generate xp.pyi stub file from all XP*.pyi files in the current directory.

This script:
1. Renames existing xp.pyi to xp.pyi.sv
2. Parses all XP*.pyi files to extract functions, classes, constants, and enums
3. Generates a new xp.pyi file with proper type hints
"""

import ast
import os
import sys
from pathlib import Path
from typing import Set, List, Dict, Any
from collections import OrderedDict


class StubGenerator:
    """Generate Python stub files from source code."""

    def __init__(self):
        self.imports: Set[str] = set()
        self.from_imports: Dict[str, Set[str]] = {}
        self.constants: List[str] = []
        self.functions: List[tuple[str, str | None]] = []  # (signature, docstring)
        self.classes: List[str] = []
        self.enums: List[str] = []
        self.used_types: Set[str] = set()  # Track which types from xp_typing are used
        self.xp_typing_defs: Dict[str, str] = {}  # Store xp_typing definitions

    def add_import(self, module: str):
        """Add a regular import."""
        self.imports.add(module)

    def add_from_import(self, module: str, names: List[str]):
        """Add a from...import statement."""
        if module not in self.from_imports:
            self.from_imports[module] = set()
        self.from_imports[module].update(names)

    def format_arg(self, arg: ast.arg, default: ast.expr | None = None) -> str:
        """Format a function argument with its annotation and optional default value."""
        if arg.annotation:
            arg_str = f"{arg.arg}: {ast.unparse(arg.annotation)}"
        else:
            arg_str = arg.arg

        if default is not None:
            default_str = ast.unparse(default)
            arg_str += f"={default_str}"

        return arg_str

    def get_docstring(self, node: ast.FunctionDef) -> str | None:
        """Extract docstring from function definition."""
        if (node.body and
            isinstance(node.body[0], ast.Expr) and
            isinstance(node.body[0].value, ast.Constant) and
            isinstance(node.body[0].value.value, str)):
            return node.body[0].value.value
        return None

    def get_function_signature(self, node: ast.FunctionDef) -> tuple[str, str | None]:
        """Extract function signature and docstring as a tuple."""
        args = []

        # Calculate which regular args have defaults
        # Defaults are right-aligned with args list
        num_defaults = len(node.args.defaults)
        num_args = len(node.args.args)
        defaults_start_idx = num_args - num_defaults

        # Handle regular arguments
        for i, arg in enumerate(node.args.args):
            default = None
            if i >= defaults_start_idx:
                default = node.args.defaults[i - defaults_start_idx]
            args.append(self.format_arg(arg, default))

        # Handle *args
        if node.args.vararg:
            vararg = self.format_arg(node.args.vararg)
            args.append(f"*{vararg}")

        # Handle keyword-only arguments
        if node.args.kwonlyargs:
            if not node.args.vararg:
                args.append("*")
            for arg, kw_default in zip(node.args.kwonlyargs, node.args.kw_defaults):
                args.append(self.format_arg(arg, kw_default))

        # Handle **kwargs
        if node.args.kwarg:
            kwarg = self.format_arg(node.args.kwarg)
            args.append(f"**{kwarg}")

        args_str = ", ".join(args)

        # Handle return type
        if node.returns:
            return_type = ast.unparse(node.returns)
            signature = f"def {node.name}({args_str}) -> {return_type}: ..."
        else:
            signature = f"def {node.name}({args_str}): ..."

        # Get docstring
        docstring = self.get_docstring(node)

        return signature, docstring

    def process_class(self, node: ast.ClassDef) -> str:
        """Process a class definition."""
        # Get base classes
        bases = []
        for base in node.bases:
            bases.append(ast.unparse(base))

        if bases:
            class_def = f"class {node.name}({', '.join(bases)}):"
        else:
            class_def = f"class {node.name}:"

        lines = [class_def]

        # Check if class has any methods or class variables
        has_members = False

        for item in node.body:
            if isinstance(item, ast.FunctionDef):
                has_members = True
                method_sig, _ = self.get_function_signature(item)
                lines.append(f"    {method_sig}")
            elif isinstance(item, ast.AnnAssign) and isinstance(item.target, ast.Name):
                # Class variable with annotation
                has_members = True
                var_name = item.target.id
                if item.annotation:
                    type_str = ast.unparse(item.annotation)
                    lines.append(f"    {var_name}: {type_str}")
            elif isinstance(item, ast.Assign):
                # Class variable without annotation
                for target in item.targets:
                    if isinstance(target, ast.Name):
                        has_members = True
                        lines.append(f"    {target.id}: Any")

        # If class has no members, add pass
        if not has_members:
            lines.append("    pass")

        return "\n".join(lines)

    def process_constant(self, node: ast.Assign) -> List[str]:
        """Process a module-level constant assignment."""
        constants = []
        for target in node.targets:
            if isinstance(target, ast.Name):
                # Try to infer type from value
                value_type = "Any"
                if isinstance(node.value, ast.Constant):
                    if isinstance(node.value.value, int):
                        value_type = "int"
                    elif isinstance(node.value.value, float):
                        value_type = "float"
                    elif isinstance(node.value.value, str):
                        value_type = "str"
                    elif isinstance(node.value.value, bool):
                        value_type = "bool"
                constants.append(f"{target.id}: {value_type}")
        return constants

    def process_annotated_assign(self, node: ast.AnnAssign) -> str:
        """Process an annotated assignment."""
        if isinstance(node.target, ast.Name):
            type_str = ast.unparse(node.annotation)
            return f"{node.target.id}: {type_str}"
        return ""

    def extract_imports(self, node: ast.AST):
        """Extract import statements from AST."""
        if isinstance(node, ast.Import):
            for alias in node.names:
                self.add_import(alias.name)
        elif isinstance(node, ast.ImportFrom):
            if node.module:
                # Skip XPPython3.xp_typing imports - we'll handle those specially
                if node.module == 'XPPython3.xp_typing':
                    return
                names = [alias.name for alias in node.names]
                self.add_from_import(node.module, names)

    def track_used_types(self, code: str):
        """Track which types from xp_typing are used in the code."""
        # Look for type names that might be from xp_typing
        for type_name in self.xp_typing_defs.keys():
            if type_name in code:
                self.used_types.add(type_name)

    def parse_file(self, filepath: Path):
        """Parse a Python file and extract stub information."""
        with open(filepath, 'r', encoding='utf-8') as f:
            try:
                content = f.read()
                tree = ast.parse(content, filename=str(filepath))
            except SyntaxError as e:
                print(f"Warning: Syntax error in {filepath}: {e}", file=sys.stderr)
                return

        # Track used types from this file
        self.track_used_types(content)

        for node in ast.walk(tree):
            self.extract_imports(node)

        for node in tree.body:
            if isinstance(node, ast.ClassDef):
                # Check if it's an Enum
                is_enum = any(
                    isinstance(base, ast.Name) and 'Enum' in base.id
                    for base in node.bases
                )

                class_stub = self.process_class(node)
                if is_enum:
                    self.enums.append(class_stub)
                else:
                    self.classes.append(class_stub)

            elif isinstance(node, ast.FunctionDef):
                signature, docstring = self.get_function_signature(node)
                self.functions.append((signature, docstring))

            elif isinstance(node, ast.Assign):
                constants = self.process_constant(node)
                self.constants.extend(constants)

            elif isinstance(node, ast.AnnAssign):
                constant = self.process_annotated_assign(node)
                if constant:
                    self.constants.append(constant)

    def load_xp_typing(self, xp_typing_path: Path):
        """Load and parse xp_typing.py to extract type definitions."""
        with open(xp_typing_path, 'r', encoding='utf-8') as f:
            content = f.read()
            tree = ast.parse(content, filename=str(xp_typing_path))

        for node in tree.body:
            if isinstance(node, ast.Assign):
                # Handle NewType assignments like: XPLMDataRef = NewType('XPLMDataRef', int)
                for target in node.targets:
                    if isinstance(target, ast.Name):
                        type_name = target.id
                        self.xp_typing_defs[type_name] = ast.unparse(node)
            elif isinstance(node, ast.ClassDef):
                # Handle dataclass and regular class definitions
                class_def = ast.unparse(node)
                self.xp_typing_defs[node.name] = class_def

    def generate_stub(self) -> str:
        """Generate the complete stub file content."""
        lines = []

        # Add file header
        lines.append("# Type stub file generated from XP*.pyi files")
        lines.append("# This file contains type hints for all functions, classes, and constants")
        lines.append("")

        # Add imports
        # Collect required typing imports
        all_typing = {'Any', 'Callable', 'Optional', 'Literal', 'Sequence'}

        # Add typing imports from collected imports
        typing_items = self.from_imports.get('typing', set())
        if typing_items:
            all_typing.update(typing_items)

        lines.append(f"from typing import {', '.join(sorted(all_typing))}")

        # Add enum import if we have enums
        if self.enums:
            lines.append("from enum import IntEnum")

        # Add explicit imports from xp_typing for used types
        if self.used_types:
            # Sort the types for consistent output
            sorted_types = sorted(self.used_types)

            # Format imports nicely - if too many, split across multiple lines
            if len(sorted_types) > 5:
                lines.append("from XPPython3.xp_typing import (")
                for i, type_name in enumerate(sorted_types):
                    if i < len(sorted_types) - 1:
                        lines.append(f"    {type_name},")
                    else:
                        lines.append(f"    {type_name}")
                lines.append(")")
            else:
                lines.append(f"from XPPython3.xp_typing import {', '.join(sorted_types)}")

        # Add other from imports (excluding typing and dataclasses to avoid duplicates)
        for module, names in sorted(self.from_imports.items()):
            if module not in ('typing', 'dataclasses'):
                lines.append(f"from {module} import {', '.join(sorted(names))}")

        # Add regular imports
        for module in sorted(self.imports):
            lines.append(f"import {module}")

        lines.append("")

        # Add constants
        if self.constants:
            c = set(self.constants)
            self.constants = sorted(list(c))
            lines.append("# Constants")
            for constant in self.constants:
                lines.append(constant)
            lines.append("")

        # Add enums
        if self.enums:
            lines.append("# Enumerations")
            for enum in self.enums:
                lines.append(enum)
                lines.append("")

        # Add classes
        if self.classes:
            lines.append("# Classes")
            for cls in self.classes:
                lines.append(cls)
                lines.append("")

        # Add functions
        if self.functions:
            lines.append("# Functions")
            for signature, docstring in self.functions:
                if docstring:
                    # Clean up docstring - remove leading/trailing blank lines and normalize indentation
                    doc_lines = docstring.split('\n')

                    # Remove leading blank lines
                    while doc_lines and not doc_lines[0].strip():
                        doc_lines.pop(0)

                    # Remove trailing blank lines
                    while doc_lines and not doc_lines[-1].strip():
                        doc_lines.pop()

                    # Format docstring with proper indentation
                    lines.append(signature.replace(': ...', ':'))
                    lines.append('    """')
                    for line in doc_lines:
                        if line.strip():
                            lines.append(f"    {line}")
                        else:
                            lines.append('')
                    lines.append('    """')
                    lines.append('    ...')
                else:
                    lines.append(signature)
            lines.append("")

        return "\n".join(lines)


def main():
    """Main entry point."""
    script_dir = Path(__file__).parent

    # Step 1: Rename existing xp.pyi to xp.pyi.sv
    old_pyi = script_dir / "xp.pyi"
    backup_pyi = script_dir / "xp.pyi.sv"

    if old_pyi.exists():
        print(f"Renaming {old_pyi} to {backup_pyi}")
        if backup_pyi.exists():
            print(f"Warning: {backup_pyi} already exists, will be overwritten")
        old_pyi.rename(backup_pyi)
    else:
        print(f"Warning: {old_pyi} does not exist")

    # Step 2: Find all XP*.pyi files (excluding xp.pyi itself)
    xp_files = sorted([f for f in script_dir.glob("XP*.pyi") if f.name != "xp.pyi"])

    if not xp_files:
        print("No XP*.pyi files found in current directory")
        return 1

    print(f"Found {len(xp_files)} XP*.pyi files:")
    for f in xp_files:
        print(f"  - {f.name}")

    # Step 3: Load xp_typing definitions
    generator = StubGenerator()
    xp_typing_file = script_dir / "xp_typing.py"

    if xp_typing_file.exists():
        print(f"Loading type definitions from {xp_typing_file.name}...")
        generator.load_xp_typing(xp_typing_file)
        print(f"  Found {len(generator.xp_typing_defs)} type definitions")
    else:
        print(f"Warning: {xp_typing_file} not found, types will not be resolved")

    # Step 4: Parse all XP*.pyi files and collect stub information
    for filepath in xp_files:
        print(f"Processing {filepath.name}...")
        generator.parse_file(filepath)

    # Step 5: Generate stub file
    stub_content = generator.generate_stub()

    # Step 6: Write new xp.pyi
    new_pyi = script_dir / "xp.pyi"
    print(f"\nWriting {new_pyi}")
    with open(new_pyi, 'w', encoding='utf-8') as f:
        f.write(stub_content)

    print(f"\nSuccessfully generated {new_pyi}")
    print(f"  - {len(generator.used_types)} types from xp_typing")
    print(f"  - {len(generator.constants)} constants")
    print(f"  - {len(generator.enums)} enums")
    print(f"  - {len(generator.classes)} classes")
    print(f"  - {len(generator.functions)} functions")

    return 0


if __name__ == "__main__":
    sys.exit(main())
