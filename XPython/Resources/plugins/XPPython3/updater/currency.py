from typing import Self
import XPPython3.updater.version as version


class Currency:
    def __init__(self: Self, self_version: str, beta_version: str, new_version: str) -> None:
        self.self_version = self_version  # self.Version
        self.beta_version = beta_version  # self.beta_version
        self.new_version = new_version    # self.new_version

    def get_currency(self: Self, try_beta: bool) -> str:
        uptodate, change_to = version.calc_update(try_beta, self.self_version, self.new_version, self.beta_version)
        if uptodate:
            if self.self_version == self.new_version:
                return f"Stable {self.self_version} is up-to-date"
            if self.self_version == self.beta_version:
                return f"Beta {self.self_version} is up-to-date"
            return f"{self.self_version} is up-to-date"

        if change_to == "Unknown":
            return "Currency not checked"
        return f"{'Beta' if change_to == self.beta_version else 'Stable'} {change_to} is available"
