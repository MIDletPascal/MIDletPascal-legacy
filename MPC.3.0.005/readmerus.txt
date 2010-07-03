Модифицированный компилятор MidletPascal.

1. переписан лексический анализатор: "sources\lex\lex.yy.c" 

2. запись строк через: 'text'#13#10'text'#9#13#10'text' 

3. команды: shr shl

4. inline вставки кода:
1-я модификация: inline(b : byte); - прямая запись байта в конструируемый код.
2-я модификация: inline(type_str, param1_str, param2_str, param3_str : string); - конструктор указателя на метод или поле в константном пуле, все параметры должны быть строками текста, (строковые переменные не допускаются).
где type_str:
"getstatic" получение статического поля класса
"putstatic" установка статического поля в классе
"getfield" перенос поля из объекта
"putfield" установка поля в объекте
"invokevirtual" вызывает метод экземпляра, основываясь на типе времени выполнения
"invokenonvirtual" вызывает метод экземпляра, основываясь на не виртуальном типе
"invokestatic" вызов метода класса (статического метода)
Пример
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
Компиляция:
1. Запустить "run.bat"
2. После успешной компиляции все появившиеся "*.class" файлы упаковать в архив "NewProject.jar.zip"
3. Переименовываем "NewProject.jar.zip" в файл "NewProject.jar"
4. Если в программе используется "fixed-point", то в архиве переименовать какой-либо из файлов  "F.big.class", "F.small.class" в файл "F.class"
5. Запускаем получившийся мидлет

arT(c). 2009
e-mail: IronWoodcutter@bk.ru
