# JsonParse
使用C++语言编写的简单的字符串转Json的解析器,方法上使用了类似于递归下降的分析方法(因为我并不是按照非终结符编写递归的,是按照终结符token如左边界来识别对象的,理论上不能完全算递归下降方法),程序实现上比较简单.