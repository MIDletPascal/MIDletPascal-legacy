���������������� ���������� MidletPascal.

1. ��������� ����������� ����������: "sources\lex\lex.yy.c" 

2. ������ ����� �����: 'text'#13#10'text'#9#13#10'text' 

3. �������: shr shl

4. ���������� ����� ��������=32767, (MP ���� ����. ����� 16000)

5. inline ������� ����:
asm-��� ������������ ����� ";" �� ����� �����, ����� �������� �� ����� ��� ������.
����� ����� ����� ���������, ������� ����������� ��� ������ ������� (����� / ������_��_�����������_���_�_���������_����_������)
��������_����_������: int / utf8 / str / class / nametype / field / method
�������� ���������� ������ ������������� ��:
1 �������� ��-�
2-� �������� ��-�
4-� �������� ��-�
1 ���� - ������ �����.����
2 ���� - ������ �����.����
��� ������ �����.����
��������� ���� ������������ ����

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
  memclean;
end.

6. ��������� ��������� ����������:
	{$R-} - fixed point (������������ �� ���������)
	{$R+} - float point
���������)

=================
����������:
1. ��������� "run.bat"
2. ����� �������� ���������� ��� ����������� "*.class" ����� ��������� � ����� "NewProject.jar.zip"
3. ��������������� "NewProject.jar.zip" � ���� "NewProject.jar"
4. ���� � ��������� ������������ "fixed-point", �� � ������ ������������� �����-���� �� ������  "F.big.class", "F.small.class" � ���� "F.class"
5. ��������� ������������ ������

���������:
���� ��� ���������� ��������� �������� � �������, �� ��� ��-�� �������� ��������� ������.
����� ��������� ��������� ���������� �� ����� MPC.EXE: ������ ������� ���� - �������� - ������������� - ��������� ��������� � ������ ������������� � Windows 95


arT(c). 2009
e-mail: IronWoodcutter@bk.ru
