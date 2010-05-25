Модифицированный компилятор MidletPascal.

1. переписан лексический анализатор: "sources\lex\lex.yy.c" 

2. запись строк через: 'text'#13#10'text'#9#13#10'text' 

3. команды: shr shl

4. Поправлена длина массивов=32767, (MP была макс. длина 16000)

5. inline вставки кода:
asm-код записывается через ";" на любую длину, может состоять из байта или опкода.
опкод может иметь параметры, которые проверяются для данной команды (число / ссылка_на_константный_пул_с_указанием_типа_записи)
указание_типа_записи: int / utf8 / str / class / nametype / field / method
проверка параметров опкода производиться на:
1 байтовый эл-т
2-х байтовый эл-т
4-х байтовый эл-т
1 байт - индекс конст.пула
2 байт - индекс конст.пула
тип записи конст.пула
параметры типа константного пула

Пример
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

6. Добавлены директивы компиляции:
	{$R-} - fixed point (задействован по умолчанию)
	{$R+} - float point
умолчанию)

=================
Компиляция:
1. Запустить "run.bat"
2. После успешной компиляции все появившиеся "*.class" файлы упаковать в архив "NewProject.jar.zip"
3. Переименовываем "NewProject.jar.zip" в файл "NewProject.jar"
4. Если в программе используется "fixed-point", то в архиве переименовать какой-либо из файлов  "F.big.class", "F.small.class" в файл "F.class"
5. Запускаем получившийся мидлет

Неполадки:
Если при компиляции программа вылетает с ошибкой, то это из-за операций выделения памяти.
Чтобы устранить неполадку необходимо на файле MPC.EXE: правой кнопкой мыши - свойства - совместимость - запустить программу в режиме совместимости с Windows 95


arT(c). 2009
e-mail: IronWoodcutter@bk.ru
