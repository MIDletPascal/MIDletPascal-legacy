���������������� ���������� MIDletPascal.  v3.0.011  ��  03.08.10
����������:
1. ��������� "run.bat"
2. ����� �������� ���������� ��� ����������� "*.class" ����� ��������� � ����� "NewProject.jar.zip"
3. ��������� ������� � ���� "NewProject.jar.zip" �������, ������� ������������ � ������� ����� ���������� ������� ������, ��������: "<<RS.class"  (Float1.class ������������� ����� F.class; Float2 ������������� ������ Real*.class)
4. ���� ����������, �������� � ����� ������� ��� ���������
5. ��������������� "NewProject.jar.zip" � ���� "NewProject.jar"
6. ��������� ������������ ������

���������:
* ���� ��� ���������� ��������� �������� � �������, �� ��� ��-�� �������� ��������� ������.
����� ��������� ��������� ���������� �� ����� MPC.EXE: ������ ������� ���� - �������� - ������������� - ��������� ��������� � ������ ������������� � Windows 95

* ���� � ��������� ����������� ������� ������, ���������� ��������� ��� � ��������� Utf-8.

* MPC �� ������� ������ ���������� �������, ������� ������� ���������� �� �������������� ����: ������� ������������ ������������ ������, �� ������������ �� ����� �� ����� �������; ����� ������������ ��, ������� ���������� ���, � �.�... � ���������� ������������� �������� ���������.


======================================
����������� / ����������:

1. ��������� ����������� ����������: "sources\lex\lex.yy.c" 

2. ������ ����� �����: 'text'#13#10'text'#9#13#10'text' 

3. ��������� �������: shr shl

4. ������ ����������� ����� ��������, (����. ����� ���� 16000 ���������)

5. inline ������� ����:
asm-��� ������������ ����� ";" �� ����� �����, ����� �������� �� ����� ��� ������.
����� ����� ����� ���������, ������� ����������� ��� ������ ������� (����� / ������_��_�����������_���_�_���������_����_������)
��������_����_������: int / utf8 / str / class / nametype / field / method / interface
�������� ���������� ������ ������������� ��:
1 �������� ��-�
2-� �������� ��-�
4-� �������� ��-�
1 ���� - ������ �����.����
2 ���� - ������ �����.����
��� ������ �����.����
��������� ���� ������������ ����

������� ������������ ������� ����� ������������ ��� � ������������� ���������, ��� � � ��������� �����.
����� � ���������� ����� ���� ������ ������, �������� ����������� ����� ���������,
���� :123; ��� � hex-���� :$123abc; ����� �������� ����� ����������� ������� ������ ����� � �������.
������� ������
goto :100; - ������� �� ����� � ������ 100
goto 100; - ������� �� �������������� �������� �� 100 ������ ������ (���� �������� �������������, �� ���������� ������������ � ���� $FFFF,FFFE, � ��.)


������
var
  myint : integer;

procedure memclean;
begin
  inline(invokestatic  method  'java/lang/System', 'gc', '()V');
end;

begin
  //myint:=10;
  inline(  //inline demo1
    16; $10;
    putstatic		field	'M','myint','I'
  ) ;
  //myint:=getWidth;
    inline(
    getstatic	field	'M', 'I', 'Ljavax/microedition/lcdui/Image;';		//rem1
    invokevirtual	method	'javax/microedition/lcdui/Image', 'getWidth', '()I';		{rem2}
    putstatic	field	'M','myint','I'					(*rem3*)
  );
  //label
  inline(
    goto :1;
    nop;
    nop;
:1; nop;
    nop;
  );
  memclean;
end.

6. ��������� ��������� ����������:
	{$R-}	- fixed point (������������ �� ���������)
	{$R+}	- float point
	{$T-}	- ��������� lowcase ������ ���������
	{$T+}	- �������� lowcase ������ ��������� (������������ �� ���������)
	{$C-}	- canvas is NORMAL (������������ �� ���������)
	{$C+}	- canvas is FULL_MIDP20
	{$C*}	- canvas is FULL_NOKIA
����������:
���������  float point � canvas ���������� ����������� � ����� ������ �����, ����� ���������� ������� � ��������� ������ ����������� ����������, ���� �� ��������� ����� fixed point, �� ��� ����� ��������� ������ ��������������� ����� ����������� ���������� ��� ����� ������������ � ���.
������ lowcase - ���������� ��� �������� � ������� � ����������, ������� �������� ����� �������� ��������. (�������� �������� ������ � ������� � ������ "Lib_....class")

7. �������� ���������� ��������� ������. ��������� ����� ��� ������������ ��������� � ��������, � ����� ������ ��� ��������� ���������� ����������.

8. �� ����� ������ ������������ ����� ������ �������������� �����, ��� ��������� ������������ ���������� �������, ��� ��������� ������ �����������.


�� ���� ��������� ������� ������ MPC ������� ��������� �� e-mail: ��������� �� ������, �������� ��� � ������� ������������, ����� ������ �����������.

======================================



arT(c). 2009-2010
e-mail: IronWoodcutter@bk.ru
