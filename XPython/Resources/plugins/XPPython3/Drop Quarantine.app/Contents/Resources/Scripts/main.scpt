FasdUAS 1.101.10   ��   ��    k             x     �� ����    4    �� 
�� 
frmk  m     	 	 � 
 
  F o u n d a t i o n��        x    �� ����    2   ��
�� 
osax��        l     ��������  ��  ��        j    �� �� 0 nspasteboard NSPasteboard  N       n       o    ���� 0 nspasteboard NSPasteboard  m    ��
�� misccura      j    "�� �� 0 nsurl NSURL  N    !   n        o     ���� 0 nsurl NSURL  m    ��
�� misccura      l     ��������  ��  ��        i   # &   !   I      �� "���� 0 	nonefound 	noneFound "  #�� # o      ���� 
0 errmsg  ��  ��   ! k      $ $  % & % l     �� ' (��   ' B < terminating case: we didn't find _any_ files to remove from    ( � ) ) x   t e r m i n a t i n g   c a s e :   w e   d i d n ' t   f i n d   _ a n y _   f i l e s   t o   r e m o v e   f r o m &  * + * l     �� , -��   , C = quarantine. Assume this is really an error, so tell the user    - � . . z   q u a r a n t i n e .   A s s u m e   t h i s   i s   r e a l l y   a n   e r r o r ,   s o   t e l l   t h e   u s e r +  / 0 / l     �� 1 2��   1 ' ! how to this script and then exit    2 � 3 3 B   h o w   t o   t h i s   s c r i p t   a n d   t h e n   e x i t 0  4 5 4 I    �� 6 7
�� .sysodlogaskr        TEXT 6 b      8 9 8 m      : : � ; ;t F o u n d   n o   q u a r a n t i n e d   f i l e s . 
 M o v e   t h i s   s c r i p t   i n t o   t h e   f o l d e r   f o r   t h e   p l u g i n   y o u   w h i c h   t o 
 a u t h o r i z e . . .   < X - P l a n e > / R e s o u r c e s / p l u g i n s / X X X X X / 
 O r ,   d r o p   a   f o l d e r   o r   f i l e   o n   t h i s   a p p l i c a t i o n . 	 
 
 9 o    ���� 
0 errmsg   7 �� <��
�� 
btns < J     = =  >�� > m     ? ? � @ @  Q u i t��  ��   5  A�� A I   ������
�� .aevtquitnull��� ��� null��  ��  ��     B C B l     ��������  ��  ��   C  D E D l     ��������  ��  ��   E  F G F l     ��������  ��  ��   G  H I H l     ��������  ��  ��   I  J K J i   ' * L M L I      �� N���� 0 	authorize   N  O P O o      ����  0 thecurrentitem theCurrentItem P  Q�� Q o      ���� 0 islast isLast��  ��   M k     R R  S T S I    �� U��
�� .sysodlogaskr        TEXT U b      V W V m      X X � Y Y F A u t h o r i z i n g   M a c   * . x p l   p l u g i n s   i n :   
 W o    ����  0 thecurrentitem theCurrentItem��   T  Z [ Z l   ��������  ��  ��   [  \ ] \ l   �� ^ _��   ^ 4 . there are two methods to actually do the work    _ � ` ` \   t h e r e   a r e   t w o   m e t h o d s   t o   a c t u a l l y   d o   t h e   w o r k ]  a b a l   �� c d��   c ^ X the first is the faster of the two, removing the quarantime attribute in one quick shot    d � e e �   t h e   f i r s t   i s   t h e   f a s t e r   o f   t h e   t w o ,   r e m o v i n g   t h e   q u a r a n t i m e   a t t r i b u t e   i n   o n e   q u i c k   s h o t b  f g f l   �� h i��   h ` Z this works well when there is a relatively small number of files (say, <20). The downside    i � j j �   t h i s   w o r k s   w e l l   w h e n   t h e r e   i s   a   r e l a t i v e l y   s m a l l   n u m b e r   o f   f i l e s   ( s a y ,   < 2 0 ) .   T h e   d o w n s i d e g  k l k l   �� m n��   m X R is we give no feedback to the user with respect to progress. If there are a large    n � o o �   i s   w e   g i v e   n o   f e e d b a c k   t o   t h e   u s e r   w i t h   r e s p e c t   t o   p r o g r e s s .   I f   t h e r e   a r e   a   l a r g e l  p q p l   �� r s��   r X R number of files (say, 2000+) this can take a measurable amount of time and may be    s � t t �   n u m b e r   o f   f i l e s   ( s a y ,   2 0 0 0 + )   t h i s   c a n   t a k e   a   m e a s u r a b l e   a m o u n t   o f   t i m e   a n d   m a y   b e q  u v u l   �� w x��   w _ Y confusing to a user who may think nothing is going on, or perhaps the script has already    x � y y �   c o n f u s i n g   t o   a   u s e r   w h o   m a y   t h i n k   n o t h i n g   i s   g o i n g   o n ,   o r   p e r h a p s   t h e   s c r i p t   h a s   a l r e a d y v  z { z l   �� | }��   |   completed.    } � ~ ~    c o m p l e t e d . {   �  l   ��������  ��  ��   �  � � � l   �� � ���   � [ U So, we use the second method, whereby we remove the quarantine attribute one file at    � � � � �   S o ,   w e   u s e   t h e   s e c o n d   m e t h o d ,   w h e r e b y   w e   r e m o v e   t h e   q u a r a n t i n e   a t t r i b u t e   o n e   f i l e   a t �  � � � l   �� � ���   � F @ a time, but also provide on-going progress updates to the user.    � � � � �   a   t i m e ,   b u t   a l s o   p r o v i d e   o n - g o i n g   p r o g r e s s   u p d a t e s   t o   t h e   u s e r . �  � � � Z    � � ��� � � m    	��
�� boovfals � k    # � �  � � � l   �� � ���   � , & the fast way, with no progress update    � � � � L   t h e   f a s t   w a y ,   w i t h   n o   p r o g r e s s   u p d a t e �  � � � r     � � � I   �� ���
�� .sysoexecTEXT���     TEXT � b     � � � b     � � � m     � � � � �  f i n d   ' � o    ����  0 thecurrentitem theCurrentItem � m     � � � � � h '   - x a t t r n a m e   c o m . a p p l e . q u a r a n t i n e   - p r i n t     \ ;   |   w c   - l��   � o      ���� 0 	the_count   �  ��� � r    # � � � I   !�� ���
�� .sysoexecTEXT���     TEXT � b     � � � b     � � � m     � � � � �  f i n d   ' � o    ����  0 thecurrentitem theCurrentItem � m     � � � � � � '   - x a t t r n a m e   c o m . a p p l e . q u a r a n t i n e   - p r i n t   - e x e c   x a t t r   - d   c o m . a p p l e . q u a r a n t i n e   { }   \ ;   |   w c   - l��   � o      ���� 0 
the_result  ��  ��   � k   & � � �  � � � l  & &�� � ���   � . ( the slow way, but with progress updates    � � � � P   t h e   s l o w   w a y ,   b u t   w i t h   p r o g r e s s   u p d a t e s �  � � � r   & 1 � � � I  & /�� ���
�� .sysoexecTEXT���     TEXT � b   & + � � � b   & ) � � � m   & ' � � � � �  f i n d   ' � o   ' (����  0 thecurrentitem theCurrentItem � m   ) * � � � � � P '   - x a t t r n a m e   c o m . a p p l e . q u a r a n t i n e   - p r i n t��   � o      ���� 0 allfiles allFiles �  � � � r   2 7 � � � n   2 5 � � � 2  3 5��
�� 
cpar � o   2 3���� 0 allfiles allFiles � l      ����� � o      ���� 0 list_of_files  ��  ��   �  � � � r   8 = � � � n   8 ; � � � 1   9 ;��
�� 
leng � o   8 9���� 0 list_of_files   � o      ���� 0 	the_count   �  � � � l  > >��������  ��  ��   �  � � � r   > C � � � o   > ?���� 0 	the_count   � 1   ? B��
�� 
ppgt �  � � � r   D I � � � m   D E����   � 1   E H��
�� 
ppgc �  � � � r   J Y � � � b   J S � � � b   J Q � � � b   J O � � � b   J M � � � m   J K � � � � �  U p d a t i n g   � o   K L���� 0 	the_count   � m   M N � � � � �    i t e m s   i n   � o   O P����  0 thecurrentitem theCurrentItem � m   Q R � � � � �  : � 1   S X��
�� 
ppgd �  � � � r   Z c � � � m   Z ] � � � � �   � 1   ] b��
�� 
ppga �  � � � X   d � ��� � � k   x � � �  � � � r   x � � � � [   x } � � � 1   x {��
�� 
ppgc � m   { |����  � 1   } ���
�� 
ppgc �  ��� � Z   � � � ����� � A  � � �  � l  � ����� n   � � 1   � ���
�� 
leng o   � �����  0 thecurrentitem theCurrentItem��  ��    l  � ����� n   � � 1   � ���
�� 
leng o   � ����� 0 thefile theFile��  ��   � k   � � 	 r   � �

 n   � � 7  � ���
�� 
ctxt l  � ����� [   � � m   � �����  l  � ����� n   � � 1   � ���
�� 
leng o   � ���  0 thecurrentitem theCurrentItem��  ��  ��  ��   l  � ��~�} n   � � 1   � ��|
�| 
leng o   � ��{�{ 0 thefile theFile�~  �}   o   � ��z�z 0 thefile theFile o      �y�y 0 theshortfile theShortFile	  r   � � o   � ��x�x 0 theshortfile theShortFile 1   � ��w
�w 
ppga �v I  � ��u�t
�u .sysoexecTEXT���     TEXT b   � �  b   � �!"! m   � �## �$$ > x a t t r   - d   c o m . a p p l e . q u a r a n t i n e   '" o   � ��s�s 0 thefile theFile  m   � �%% �&&  '�t  �v  ��  ��  ��  �� 0 thefile theFile � o   g h�r�r 0 list_of_files   � '(' r   � �)*) o   � ��q�q 0 	the_count  * o      �p�p 0 
the_result  ( +,+ r   � �-.- m   � �// �00  . 1   � ��o
�o 
ppgd, 121 r   � �343 m   � ��n�n  4 1   � ��m
�m 
ppgt2 565 r   � �787 m   � �99 �::  8 1   � ��l
�l 
ppga6 ;�k; r   � �<=< m   � ��j�j  = 1   � ��i
�i 
ppgc�k   � >?> l  � ��h�g�f�h  �g  �f  ? @A@ Z   �BC�e�dB F   � �DED =  � �FGF c   � �HIH o   � ��c�c 0 
the_result  I m   � ��b
�b 
nmbrG m   � ��a�a  E H   � �JJ o   � ��`�` 0 islast isLastC I  �	�_KL
�_ .sysodlogaskr        TEXTK b   � �MNM m   � �OO �PP F N o   q u a r a n t i n e d   f i l e s   f o u n d   i n   
 	 	 	 
N o   � ��^�^  0 thecurrentitem theCurrentItemL �]Q�\
�] 
btnsQ J   RR S�[S m   TT �UU  O k a y�[  �\  �e  �d  A V�ZV L  WW c  XYX o  �Y�Y 0 
the_result  Y m  �X
�X 
nmbr�Z   K Z[Z l     �W�V�U�W  �V  �U  [ \]\ l     �T�S�R�T  �S  �R  ] ^_^ l     �Q�P�O�Q  �P  �O  _ `a` i   + .bcb I      �Nd�M�N 0 iamdone iAmDoned e�Le o      �K�K 0 numfiles numFiles�L  �M  c k     ff ghg l     �Jij�J  i %  we're done: let the user know!   j �kk >   w e ' r e   d o n e :   l e t   t h e   u s e r   k n o w !h l�Il Z     mn�Hom >    pqp o     �G�G 0 numfiles numFilesq m    �F�F  n I   �Ers
�E .sysodlogaskr        TEXTr b    tut b    vwv m    xx �yy  S u c c e s s :  w l   
z�D�Cz c    
{|{ o    �B�B 0 numfiles numFiles| m    	�A
�A 
TEXT�D  �C  u m    }} �~~    f r e e d !s �@�?
�@ 
btns J    �� ��>� m    �� ���  O k a y�>  �?  �H  o I    �=��<�= 0 	nonefound 	noneFound� ��;� m    �� ���  �;  �<  �I  a ��� l     �:�9�8�:  �9  �8  � ��� l     �7�6�5�7  �6  �5  � ��� l     �4���4  � 6 0 MAIN SCRIPT -- this is called for dropped files   � ��� `   M A I N   S C R I P T   - -   t h i s   i s   c a l l e d   f o r   d r o p p e d   f i l e s� ��� l     �3�2�1�3  �2  �1  � ��� l     �0���0  � E ? We'll process each dropped item (one or more folders or files)   � ��� ~   W e ' l l   p r o c e s s   e a c h   d r o p p e d   i t e m   ( o n e   o r   m o r e   f o l d e r s   o r   f i l e s )� ��� l     �/���/  � 8 2 allowing the user to review and potentially skip.   � ��� d   a l l o w i n g   t h e   u s e r   t o   r e v i e w   a n d   p o t e n t i a l l y   s k i p .� ��� l     �.�-�,�.  �-  �,  � ��� l     �+�*�)�+  �*  �)  � ��� i   / 2��� I     �(��'
�( .aevtodocnull  �    alis� o      �&�& 0 aa  �'  � k     ��� ��� l     �%���%  � y s get the dragged files and folders directly off of the dragged items clipboard, ignoring the droplet's misbehaviors   � ��� �   g e t   t h e   d r a g g e d   f i l e s   a n d   f o l d e r s   d i r e c t l y   o f f   o f   t h e   d r a g g e d   i t e m s   c l i p b o a r d ,   i g n o r i n g   t h e   d r o p l e t ' s   m i s b e h a v i o r s� ��� r     ��� n    ��� I    �$��#�$ *0 pasteboardwithname_ pasteboardWithName_� ��"� l   ��!� � n   ��� o    �� ,0 nspasteboardnamedrag NSPasteboardNameDrag� m    �
� misccura�!  �   �"  �#  � o     �� 0 nspasteboard NSPasteboard� o      �� 
0 pbdrag  � ��� Z    5����� n   ��� I    ���� "0 containsobject_ containsObject_� ��� l   ���� n   ��� o    �� 20 nspasteboardtypefileurl NSPasteboardTypeFileURL� m    �
� misccura�  �  �  �  � n   ��� I    ���� 	0 types  �  �  � o    �� 
0 pbdrag  � r    1��� c    /��� l   -���� n   -��� I    -���� @0 readobjectsforclasses_options_ readObjectsForClasses_options_� ��� J    (�� ��
� n   &��� m   $ &�	
�	 
pcls� o    $�� 0 nsurl NSURL�
  � ��� l  ( )���� m   ( )�
� 
msng�  �  �  �  � o    �� 
0 pbdrag  �  �  � m   - .�
� 
list� o      �� "0 thedroppeditems theDroppedItems�  �  � ��� l  6 6� �����   ��  ��  � ��� r   6 9��� m   6 7����  � o      ���� 0 
totalfiles 
totalFiles� ��� Y   : ��������� k   H ��� ��� Q   H ����� k   K m�� ��� r   K S��� l  K Q������ n   K Q��� 1   O Q��
�� 
psxp� l  K O������ n   K O��� 4   L O���
�� 
cobj� o   M N���� 0 a  � o   K L���� "0 thedroppeditems theDroppedItems��  ��  ��  ��  � o      ����  0 thecurrentitem theCurrentItem� ��� r   T ]��� =  T [��� o   T U���� 0 a  � l  U Z������ I  U Z�����
�� .corecnte****       ****� o   U V���� "0 thedroppeditems theDroppedItems��  ��  ��  � o      ���� 0 islast isLast� ��� r   ^ g��� I   ^ e������� 0 	authorize  � ��� o   _ `����  0 thecurrentitem theCurrentItem�  ��  o   ` a���� 0 islast isLast��  ��  � o      ���� 0 numauthorized numAuthorized� �� r   h m [   h k o   h i���� 0 
totalfiles 
totalFiles o   i j���� 0 numauthorized numAuthorized o      ���� 0 
totalfiles 
totalFiles��  � R      ��
�� .ascrerr ****      � **** o      ���� 
0 errmsg   ����
�� 
errn o      ���� 0 errorno  ��  � Z   u �	
��	 =  u z o   u v���� 0 errorno   m   v y������
 k   } �  l  } }����     user pressed "cancel"    � ,   u s e r   p r e s s e d   " c a n c e l " �� I  } ���
�� .sysodlogaskr        TEXT b   } � m   } � �  S k i p p i n g   o   � �����  0 thecurrentitem theCurrentItem ����
�� 
btns J   � � �� m   � � �  O k a y��  ��  ��  ��   I   � ��� ���� 0 	nonefound 	noneFound  !��! o   � ����� 
0 errmsg  ��  ��  � "��" l  � ���������  ��  ��  ��  �� 0 a  � m   = >���� � I  > C��#��
�� .corecnte****       ****# o   > ?���� "0 thedroppeditems theDroppedItems��  ��  � $��$ I   � ���%���� 0 iamdone iAmDone% &��& o   � ����� 0 
totalfiles 
totalFiles��  ��  ��  � '(' l     ��������  ��  ��  ( )*) l     ��������  ��  ��  * +,+ l     ��-.��  - L F ALTERNATIVE SCRIPT -- this is called if application is double clicked   . �// �   A L T E R N A T I V E   S C R I P T   - -   t h i s   i s   c a l l e d   i f   a p p l i c a t i o n   i s   d o u b l e   c l i c k e d, 010 l     ��������  ��  ��  1 232 l     ��������  ��  ��  3 4��4 i   3 6565 I     ������
�� .aevtoappnull  �   � ****��  ��  6 k     377 898 O     :;: k    << =>= l   ��?@��  ? ] W get the path of the _folder_ containing this application. This is kinda tricky because   @ �AA �   g e t   t h e   p a t h   o f   t h e   _ f o l d e r _   c o n t a i n i n g   t h i s   a p p l i c a t i o n .   T h i s   i s   k i n d a   t r i c k y   b e c a u s e> BCB l   ��DE��  D X R we have to ask the finder for this particular "application file id", rather than    E �FF �   w e   h a v e   t o   a s k   t h e   f i n d e r   f o r   t h i s   p a r t i c u l a r   " a p p l i c a t i o n   f i l e   i d " ,   r a t h e r   t h a n  C GHG l   ��IJ��  I %  just asking for "my" location.   J �KK >   j u s t   a s k i n g   f o r   " m y "   l o c a t i o n .H LML l   ��NO��  N � ~ set current_path to (POSIX path of (container of (application file id "com.apple.ScriptEditor.id.Drop-Quarantine") as alias))   O �PP �   s e t   c u r r e n t _ p a t h   t o   ( P O S I X   p a t h   o f   ( c o n t a i n e r   o f   ( a p p l i c a t i o n   f i l e   i d   " c o m . a p p l e . S c r i p t E d i t o r . i d . D r o p - Q u a r a n t i n e " )   a s   a l i a s ) )M QRQ l   ��ST��  S Q K NOTE this is a change from using application file id to using (path to me)   T �UU �   N O T E   t h i s   i s   a   c h a n g e   f r o m   u s i n g   a p p l i c a t i o n   f i l e   i d   t o   u s i n g   ( p a t h   t o   m e )R VWV l   ��XY��  X [ U It appears to work in tests & perhaps apple changed something from earlier versions?   Y �ZZ �   I t   a p p e a r s   t o   w o r k   i n   t e s t s   &   p e r h a p s   a p p l e   c h a n g e d   s o m e t h i n g   f r o m   e a r l i e r   v e r s i o n s ?W [��[ r    \]\ l   ^����^ n    _`_ 1    ��
�� 
psxp` l   a����a c    bcb n    ded 1   	 ��
�� 
paree l   	f����f I   	��g��
�� .earsffdralis        afdrg  f    ��  ��  ��  c m    ��
�� 
alis��  ��  ��  ��  ] o      ���� 0 current_path  ��  ; m     hh�                                                                                  MACS  alis    @  Macintosh HD                   BD ����
Finder.app                                                     ����            ����  
 cu             CoreServices  )/:System:Library:CoreServices:Finder.app/    
 F i n d e r . a p p    M a c i n t o s h   H D  &System/Library/CoreServices/Finder.app  / ��  9 iji Z    1kl��mk C    non o    ���� 0 current_path  o m    pp �qq ( / p r i v a t e / v a r / f o l d e r sl k    rr sts l   ��uv��  u K E this is just Mac stupidity... If this file was downloaded as a zip &   v �ww �   t h i s   i s   j u s t   M a c   s t u p i d i t y . . .   I f   t h i s   f i l e   w a s   d o w n l o a d e d   a s   a   z i p   &t xyx l   ��z{��  z O I itself in quarantine, EVEN AFTER it's out of quarantine, it still thinks   { �|| �   i t s e l f   i n   q u a r a n t i n e ,   E V E N   A F T E R   i t ' s   o u t   o f   q u a r a n t i n e ,   i t   s t i l l   t h i n k sy }~} l   �����   U O it's in the quarantined folder... which is someone under /private/var/folders.   � ��� �   i t ' s   i n   t h e   q u a r a n t i n e d   f o l d e r . . .   w h i c h   i s   s o m e o n e   u n d e r   / p r i v a t e / v a r / f o l d e r s .~ ��� l   ������  � X R So, we don't continue & instead ask the user to move this droplet somewhere else.   � ��� �   S o ,   w e   d o n ' t   c o n t i n u e   &   i n s t e a d   a s k   t h e   u s e r   t o   m o v e   t h i s   d r o p l e t   s o m e w h e r e   e l s e .� ��� l   ������  � U O Does not matter where, so might as well move it to plugins folder, or Desktop.   � ��� �   D o e s   n o t   m a t t e r   w h e r e ,   s o   m i g h t   a s   w e l l   m o v e   i t   t o   p l u g i n s   f o l d e r ,   o r   D e s k t o p .� ���� I   �����
�� .sysodlogaskr        TEXT� m    �� ��� � D r o p   Q u a r a n t i n e   s t i l l   i n   o r i g i n a l   f o l d e r . 
 	 
 M o v e   i t   t o   y o u r   d e s k t o p   o r   t o   X - P l a n e / R e s o u r c e s / p l u g i n s 
 a n d   r e t r y .��  ��  ��  m k   ! 1�� ��� r   ! *��� I   ! (������� 0 	authorize  � ��� o   " #���� 0 current_path  � ���� m   # $��
�� boovtrue��  ��  � o      ���� 0 numauthorized numAuthorized� ���� I   + 1������� 0 iamdone iAmDone� ���� o   , -���� 0 numauthorized numAuthorized��  ��  ��  j ��� l  2 2��������  ��  ��  � ���� l  2 2������  � h b Okay, current_path should be the folder containing this script: display to user for confirmation.   � ��� �   O k a y ,   c u r r e n t _ p a t h   s h o u l d   b e   t h e   f o l d e r   c o n t a i n i n g   t h i s   s c r i p t :   d i s p l a y   t o   u s e r   f o r   c o n f i r m a t i o n .��  ��       
�������������  � ����~�}�|�{�z�y
�� 
pimr� 0 nspasteboard NSPasteboard�~ 0 nsurl NSURL�} 0 	nonefound 	noneFound�| 0 	authorize  �{ 0 iamdone iAmDone
�z .aevtodocnull  �    alis
�y .aevtoappnull  �   � ****� �x��x �  ��� �w��v
�w 
cobj� ��   �u 	
�u 
frmk�v  � �t��s
�t 
cobj� ��   �r
�r 
osax�s  � �� �q�p
�q misccura�p 0 nspasteboard NSPasteboard� �� �o�n
�o misccura�n 0 nsurl NSURL� �m !�l�k���j�m 0 	nonefound 	noneFound�l �i��i �  �h�h 
0 errmsg  �k  � �g�g 
0 errmsg  �  :�f ?�e�d
�f 
btns
�e .sysodlogaskr        TEXT
�d .aevtquitnull��� ��� null�j �%��kvl O*j � �c M�b�a���`�c 0 	authorize  �b �_��_ �  �^�]�^  0 thecurrentitem theCurrentItem�] 0 islast isLast�a  � �\�[�Z�Y�X�W�V�U�\  0 thecurrentitem theCurrentItem�[ 0 islast isLast�Z 0 	the_count  �Y 0 
the_result  �X 0 allfiles allFiles�W 0 list_of_files  �V 0 thefile theFile�U 0 theshortfile theShortFile�   X�T � ��S � � � ��R�Q�P�O � � ��N ��M�L�K�J�I#%/9�H�GO�FT
�T .sysodlogaskr        TEXT
�S .sysoexecTEXT���     TEXT
�R 
cpar
�Q 
leng
�P 
ppgt
�O 
ppgc
�N 
ppgd
�M 
ppga
�L 
kocl
�K 
cobj
�J .corecnte****       ****
�I 
ctxt
�H 
nmbr
�G 
bool
�F 
btns�`�%j Of �%�%j E�O�%�%j E�Y ��%�%j E�O��-E�O��,E�O�*�,FOj*�,FO��%�%�%�%*a ,FOa *a ,FO [�[a a l kh *�,k*�,FO��,��, 0�[a \[Zk��,\Z��,2E�O�*a ,FOa �%a %j Y h[OY��O�E�Oa *a ,FOj*�,FOa *a ,FOj*�,FO�a &j 	 �a & a �%a a kvl Y hO�a &� �Ec�D�C���B�E 0 iamdone iAmDone�D �A��A �  �@�@ 0 numfiles numFiles�C  � �?�? 0 numfiles numFiles� x�>}�=��<��;
�> 
TEXT
�= 
btns
�< .sysodlogaskr        TEXT�; 0 	nonefound 	noneFound�B �j ��&%�%��kvl Y *�k+ � �:��9�8���7
�: .aevtodocnull  �    alis�9 0 aa  �8  � 
�6�5�4�3�2�1�0�/�.�-�6 0 aa  �5 
0 pbdrag  �4 "0 thedroppeditems theDroppedItems�3 0 
totalfiles 
totalFiles�2 0 a  �1  0 thecurrentitem theCurrentItem�0 0 islast isLast�/ 0 numauthorized numAuthorized�. 
0 errmsg  �- 0 errorno  � �,�+�*�)�(�'�&�%�$�#�"�!� ��������
�, misccura�+ ,0 nspasteboardnamedrag NSPasteboardNameDrag�* *0 pasteboardwithname_ pasteboardWithName_�) 	0 types  �( 20 nspasteboardtypefileurl NSPasteboardTypeFileURL�' "0 containsobject_ containsObject_
�& 
pcls
�% 
msng�$ @0 readobjectsforclasses_options_ readObjectsForClasses_options_
�# 
list
�" .corecnte****       ****
�! 
cobj
�  
psxp� 0 	authorize  � 
0 errmsg  � ���
� 
errn� 0 errorno  �  ���
� 
btns
� .sysodlogaskr        TEXT� 0 	nonefound 	noneFound� 0 iamdone iAmDone�7 �b  ��,k+ E�O�j+ ��,k+  �b  �,kv�l+ �&E�Y hOjE�O ck�j 
kh  '��/�,E�O��j 
 E�O*��l+ E�O��E�W )X  �a   a �%a a kvl Y *�k+ OP[OY��O*�k+ � �6�����
� .aevtoappnull  �   � ****�  �  �  � h�����p����
�	
� .earsffdralis        afdr
� 
pare
� 
alis
� 
psxp� 0 current_path  
� .sysodlogaskr        TEXT� 0 	authorize  �
 0 numauthorized numAuthorized�	 0 iamdone iAmDone� 4� )j �,�&�,E�UO�� 
�j Y *�el+ 	E�O*�k+ OP ascr  ��ޭ