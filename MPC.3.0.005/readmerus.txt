���������������� ���������� MidletPascal.

1. ��������� ����������� ����������: "sources\lex\lex.yy.c" 

2. ������ ����� �����: 'text'#13#10'text'#9#13#10'text' 

3. �������: shr shl

4. inline ������� ����:
1-� �����������: inline(b : byte); - ������ ������ ����� � �������������� ���.
2-� �����������: inline(type_str, param1_str, param2_str, param3_str : string); - ����������� ��������� �� ����� ��� ���� � ����������� ����, ��� ��������� ������ ���� �������� ������, (��������� ���������� �� �����������).
��� type_str:
"getstatic" ��������� ������������ ���� ������
"putstatic" ��������� ������������ ���� � ������
"getfield" ������� ���� �� �������
"putfield" ��������� ���� � �������
"invokevirtual" �������� ����� ����������, ����������� �� ���� ������� ����������
"invokenonvirtual" �������� ����� ����������, ����������� �� �� ����������� ����
"invokestatic" ����� ������ ������ (������������ ������)
������
var
  myint : integer;
begin
  //myint:=10;
  inline(16); //  bipush;
  inline(10);  // byte
  inline('putstatic','M','myint','I') ;
  //myint:=getWidth;
  inline('getstatic', 'M', 'I', 'Ljavax/microedition/lcdui/Image;');
  inline('invokevirtual', 'javax/microedition/lcdui/Image', 'getWidth', '()I');
  inline('putstatic','M','myint','I') ;


=================
����������:
1. ��������� "run.bat"
2. ����� �������� ���������� ��� ����������� "*.class" ����� ��������� � ����� "NewProject.jar.zip"
3. ��������������� "NewProject.jar.zip" � ���� "NewProject.jar"
4. ���� � ��������� ������������ "fixed-point", �� � ������ ������������� �����-���� �� ������  "F.big.class", "F.small.class" � ���� "F.class"
5. ��������� ������������ ������

arT(c). 2009
e-mail: IronWoodcutter@bk.ru
