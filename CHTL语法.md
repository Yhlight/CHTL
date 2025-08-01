# 关于CHTL
CHTL是基于C++语言实现的超文本语言，其本质是为了提供一种更符合开发者编写HTML代码的方式  
使用MIT开源协议  

## 注释
在CHTL中，使用//表示注释  
使用/**/代表多行注释  
使用--代表会被生成器识别的注释  
//和/**/注释不会被生成器所识别，生成的HTML不会带有这些注释  

## 文本节点
在CHTL中，使用text { }表示一段文本  

```chtl
text
{
    "这是一段文本"
}
```

## 无修饰字面量
CHTL支持双引号字符串("")，单引号字符串('')以及没有引号的字符串  

## 元素节点
CHTL支持HTML所有的元素，无论是单标签还是双标签，还是块级，行内，行内块元素  

```chtl
html
{
    head
    {

    }

    body
    {
        div
        {
            text
            {

            }
        }

        span
        {

        }
    }
}
```

## 属性
在CHTL中，使用`属性名 : "属性值";`表示属性  

```chtl
div
{
    id: "box";
    class: "welcome";
}
```

## 局部样式块
CHTL对<style></style>进行了改进，允许开发者在元素的内部嵌套style {}，以此进行一些css操作  
包括但不限于内联样式，类选择器，id选择器，伪类选择器，伪元素选择器  

### 内联样式
你可以在style{}内部直接添加属性，这些属性会成为元素的内联样式  

```chtl
body
{
    div
    {
        // 内联样式
        style
        {
            width: 100px;
            height: 200px;
        }
    }
}
```

### 类与id选择器
你可以无需在元素内部定义类或id名，而是直接使用类或id选择器  
编译器会自动识别类名并自动添加至元素身上，然后将类CSS块自动添加至全局样式之中  
伪类选择器和伪元素选择器的行为也一样  

```chtl
head
{
    style
    {
        // 编译器自动添加相关CSS样式到此处
    }
}

body
{
    div
    {
        // class: box;  // 编译器自动添加
        style
        {
            .box  // 识别成类名
            {
                width: 100px;
            }
        }
    }
}
```

### 增强伪类伪元素选择器
你可以在style {}内部使用&:hover / &::after的语法来表示伪类或者是伪元素  
&会根据上下文推导解析为类名或id名，其中类名优先  

```chtl
head
{
    style
    {
        // 编译器自动添加相关CSS样式到此处
    }
}

body
{
    div
    {
        // class: box;  // 编译器自动添加
        style
        {
            .box  // 识别成类名
            {
                width: 100px;
            }

            &:hover // 如果前文定义了类名或id名，则会自动解析&为类名或id名，类名优先，解析完毕后的代码后添加至全局样式块
            {

            }
        }
    }
}
```

## 样式组
CHTL根据常用CSS属性的作用对象提供了一系列样式组供给开发者使用  
如Text，Flex，在样式组约束下，你仅且只能使用这些预设的属性  

### 无值样式组
无值样式组的属性不具有值，你必须显式为每一个属性添加属性值  

```chtl
style
{
    @Style Text
    {
        color: "red";
        line-height: 1.6;
    }
}
```

### 有值样式组
无值样式组的属性具有默认值，你可以直接使用  

```chtl
style
{
    @Style Text;
}
```

## 自定义
CHTL允许开发者使用`[Custom]`语法来创建自定义样式组或自定义元素  

### 自定义样式组
开发者可以使用`[Custom] @Style 组名`来创建样式组  

```chtl
[Custom] @Style DefaultText
{
	color: "black";
	line-height: 1.6;
}
```

#### 样式组的继承
自定义样式组可以继承CHTL预设的样式组  

```chtl
[Custom] @Style DefaultText
{
	@Style Text;
}
```

#### 自定义样式组的继承
CHTL支持自定义样式组继承自定义样式组  

```chtl
[Custom] @Style DefaultText
{
	@Style Text;
}

[Custom] @Style Mark
{
    @Style DefaultText;
}
```

#### 样式组的覆盖
当自定义样式组继承另一个自定义样式组或是预设样式组时  
如果存在同名属性，则根据相关书写顺序进行覆盖  

```chtl
[Custom] @Style DefaultText
{
	color: "black";
}

[Custom] @Style Mark
{
    @Style DefaultText;
    color: "Yellow";
}
```

#### 样式组的特例化
样式组允许被特例化，特例化包括但不限于，修改属性值，增加属性，删除属性，增加样式组，删除样式组  
严禁过度使用继承与增加或删除样式组功能，这可能导致难以理解  

##### 修改属性值
```chtl
[Custom] @Style DefaultText
{
    border: "yellow 1px solid";
	color: "yellow";
}

[Custom] @Style Mark
{
    // 修改某一属性值
    @Style DefaultText
    {
        border: "black 1px solid";
    }
}

style
{
    // style块内仍允许实例化
    div
    {
        @Style DefaultText
        {
            border: "none";
	        color: "black";
        }
    }
}
```

##### 增加属性值
```chtl
[Custom] @Style DefaultText
{
    border: "yellow 1px solid";
	color: "yellow";
}

[Custom] @Style Mark
{
    // 增加属性值
    @Style DefaultText
    {
        add background: "yellow";
    }
}

style
{
    // style块内仍允许实例化
    div
    {
        @Style DefaultText
        {
            add font-size: 2rem;
        }
    }
}
```

##### 删除属性值
```chtl
[Custom] @Style DefaultText
{
    border: "yellow 1px solid";
	color: "yellow";
}

[Custom] @Style Mark
{
    // 删除属性值
    @Style DefaultText
    {
        delete border;
    }
}

style
{
    // style块内仍允许实例化
    div
    {
        @Style DefaultText
        {
            delete color;
        }
    }
}
```

##### 增加样式组
```chtl
[Custom] @Style DefaultText
{
    border: "yellow 1px solid";
	color: "yellow";
}

[Custom] @Style hide
{
    display: "none";
}

[Custom] @Style Mark
{
    @Style DefaultText
    {
        add @Style hide;
    }
}

style
{
    // style块内仍允许实例化
    div
    {
        @Style DefaultText
        {
            add @Style hide;
        }
    }
}
```

##### 删除样式组
```chtl
[Custom] @Style hide
{
    display: "none";
}

[Custom] @Style DefaultText
{
    border: "yellow 1px solid";
	color: "yellow";
    @Style hide;
}

[Custom] @Style Mark
{
    @Style DefaultText
    {
        delete @Style hide;
    }
}

style
{
    // style块内仍允许实例化
    div
    {
        @Style Mark
        {
            delete @Style DefaultText;
        }
    }
}
```

### 自定义元素
开发者可以使用`[Custom] @Element 元素名`来创建元素  

```chtl
[Custom] @Element Box
{
	div
	{
		
	}

	span
	{
		
	}
}

div
{
    @Element Box;
}
```

#### 自定义元素的继承
自定义元素能够继承另一个自定义元素  

```chtl
[Custom] @Element Box
{
	div
	{
		
	}

	span
	{
		
	}
}

[Custom] Box2
{
    div
    {

    }

	@Element Box;
}
```

#### 自定义元素的覆盖
自定义元素在语法上支持覆盖原HTML元素，但并不建议使用此功能  

```chtl
[Custom] @Element span
{
	text{ }
}
```

#### 自定义元素的特例化
自定义元素允许被特例化，这些特例化包括但不限于添加样式，索引访问，增加元素与删除元素  
无论是继承还是实际使用，都支持特例化  

##### 增加样式
```chtl
[Custom] @Element Box
{
    div
    {

    }

    span
    {

    }

    div
    {

    }
}

div
{
    @Element Box
    {
        // 为span添加样式
        span
        {
            style
            {

            }
        }

        div
        {

        }

        // 为第二个div添加样式
        div
        {
            style
            {
                
            }
        }
    }
}
```

##### 索引访问
你可以使用[index]索引来访问自定义元素中的某一个元素    

```chtl
[Configuration]
{
    INDEX_INITIAL_COUNT: 0;
}

[Custom] @Element Box
{
    div
    {

    }

    span
    {

    }

    div
    {

    }
}

div
{
    @Element Box
    {
        // 访问第二个div
        div[1]
        {
        
        }
    }
}
```

##### 增加元素
```chtl
[Custom] @Element Box
{
    div
    {

    }

    span
    {

    }

    div
    {

    }
}

div
{
    @Element Box
    {
        // 在第一个div后面增加
        add div[0]
        {
        
        }
    }
}
```

##### 删除元素
```chtl
[Custom] @Element Box
{
    div
    {

    }

    span
    {

    }

    div
    {

    }
}

div
{
    @Element Box
    {
        // 删除第一个div
        delete div[0];
    }
}
```

##### 增加自定义元素继承
```chtl
[Custom] @Element Box
{
    div
    {

    }

    span
    {

    }

    div
    {

    }
}

[Custom] @Element Box2
{
    @Element Box;

    div
    {

    }
}

[Custom] @Element Box3
{
    @Element Box2;

    div
    {

    }

    // 增加Box的继承
    add @Element Box;
}

div
{
    Box
    {
        // 添加一个特例化的Box3
        add Box3
        {
            div
            {
                style
                {

                }
            }
        }
    }
}
```

##### 删除自定义元素的继承
```chtl
[Custom] @Element Box
{
    div
    {

    }

    span
    {

    }

    div
    {

    }
}

[Custom] @Element Box2
{
    @Element Box;

    div
    {

    }
}

[Custom] @Element Box3
{
    @Element Box2
    {
        // 删除对Box的继承
        delete @Element Box;
    }

    div
    {

    }
}

div
{
    @Element Box3
    {
        // 删除对Box2的继承
        delete @Element Box2;
    }
}
```

## 变量组
CHTL扩展了CSS的变量功能，允许使用[Custom] @Var创建一组变量  
变量组使用特殊的语法，使用时无需@Var前缀  

```chtl
[Custom] @Var ThemeColor
{
    // 无需像CSS一样以--为前缀
    tableColor: "rgb(255, 192, 203)";
}

style
{
    div
    {
        // 变量组使用特殊的语法，使用时无需@Var前缀，CHTL语法允许不写出@Var
        // 你也可以显性写出@Var，但实际上是不必要的
        backgroud: ThemeColor(tableColor);
    }
}
```

### 变量组的继承
变量组和自定义样式组，自定义元素一样可以继承  

```chtl
[Custom] @Var ThemeColor
{
    // 无需像CSS一样以--为前缀
    tableColor: "rgb(255, 192, 203)";
    TextColor: "black";
}

[Custom] @Var FooterColor
{
    FooterColor: "rgb(145, 155, 200)";
    @Var ThemeColor;
}
```

### 变量组的覆盖
在继承时，如果变量组之间存在同名的变量，则为覆盖操作  

```chtl
[Custom] @Var ThemeColor
{
    // 无需像CSS一样以--为前缀
    tableColor: "rgb(255, 192, 203)";
    TextColor: "black";
}

[Custom] @Var FooterColor
{
    FooterColor: "rgb(145, 155, 200)";
    TextColor: "pink";
    @Var ThemeColor;
}

->

[Custom] @Var FooterColor
{
    FooterColor: "rgb(145, 155, 200)";
    tableColor: "rgb(255, 192, 203)";
    TextColor: "black";
}
```

### 变量组的特例化
在继承时，如果你对原变量组的变量值不满意，你可以展开并修改  
除此之外，我们允许在使用时修改变量值  
上述这些修改并不会影响到原来的变量组，只是特例化  

```chtl
[Custom] @Var ThemeColor
{
    // 无需像CSS一样以--为前缀
    tableColor: "rgb(255, 192, 203)";
    TextColor: "black";
}

[Custom] @Var FooterColor
{
    FooterColor: "rgb(145, 155, 200)";
    TextColor: "pink";

    @Var ThemeColor
    {
        tableColor: "rgb(192, 133, 145)";
    }
}

style
{
    div
    {
        color: ThemeColor(FooterColor: "rgb(255, 192, 203)");
    }
}
```

## 模板
你可以使用[Template]创建模板，模板只能被继承，无法特例化，无法修改  
存在模板样式组，模板元素，模板变量组三种类型  
模板可以被相应类型的自定义继承，模板也能继承相应类型的自定义  
如果出现了同名的情况，则需要显性写出前缀，即[Custom] /  [Template]  
正常情况下，变量组不用和其他那样明确写出@Var，但是为了进行重名区分，你需要写全  
即使用[Custom] @Var XXX / [Template] @Var XXX;  


## 等号对等式
你可以在一些需要的地方使用'='代替':'  
CHTL中':'和'='是等价的  
例如变量组的特例化，配置组的配置设置，更推荐使用等号  
color: ThemeColor(FooterColor = "rgb(255, 192, 203)");  

## 原始嵌入
在CHTL中，你可以使用[Origin]表示这是一段原始的代码，这部分代码不会被CHTL处理，而是让生成器直接生成  
原始嵌入是CHTL的兼容处理机制，避免CHTL考虑不到的极端问题  
原始嵌入允许在任意节点中被解析  

### 嵌入HTML代码
```chtl
[Origin] @Html
{

}
```

### 嵌入CSS代码
```chtl
[Origin] @Style
{

}
```

### 嵌入JS代码
```chtl
[Origin] @JavaScript
{

}
```

## 全局样式块与script
全局样式块不会处理其内部的任何内容，这是由于重新实现css代码太过艰难  
script也一样，不会对内部的内容进行检查  
以此来完美支持原生CSS  
但是有一个例外，那就是[Origin]，全局样式块和script还是要检查其内部是否带有原始嵌入，然后进行正常的解析  

## 显式继承
对于原有的组合型继承，你可以使用inherit显式继承  

```chtl
[Custom] @Element Box
{

}

[Custom] @Element Box2
{
    inherit @Element Box;
}
```

## 配置组
配置组允许开发者自定义很多行为  

```chtl
[Configuration]
{
    // 索引的起始计数
    INDEX_INITIAL_COUNT = 0;

    // 自定义名称
    CUSTOM_STYLE = @Style;
    CUSTOM_ELEMENT = @Element;
    CUSTOM_VAR = @Var;
    TEMPLATE_STYLE = @Style;
    TEMPLATE_ELEMENT = @Element;
    TEMPLATE_VAR = @Var;
    ORIGIN_HTML = @Html;
    ORIGIN_STYLE = @Style;
    ORIGIN_JAVASCRIPT = @JavaScript;
    KEYWORD_ADD = add;
    KEYWORD_DELETE = delete;
    KEYWORD_INHERIT = inherit;
    KWYWORD_FROM = from;
    KEYWORD_AS = as;
    KEYWORD_TEXT = text;
    KEYWORD_STYLE = style;
    KEYWORD_CUSTOM = [Custom];
    KEYWORD_TEMPLATE = [Template];
    KEYWORD_ORIGIN = [Origin];
    KEYWORD_IMPORT = [Import]
    KEYWORD_NAMESPACE = [Namespace]

    // 是否禁用Name配置组
    DISABLE_NAME_GROUP = true;
    // DEBUG模式
    DEBUG_MODE = false;
    // 组选项数量
    OPTION_COUNT = 1;
}
```

### Name配置块与组选项(针对配置组)
允许使用[]定义一组内容，允许CHTL程序支持一组内容中的任一内容  

```chtl
[Configuration]
{
    // 不支持组选项
    INDEX_INITIAL_COUNT = 0;
    // 是否禁用Name配置组
    DISABLE_NAME_GROUP = true;
    // DEBUG模式
    DEBUG_MODE = false;

    // Name配置块
    [Name]
    {
        // 组选项
        CUSTOM_STYLE = [@Style, @style, @CSS, @Css, @css];

        CUSTOM_STYLE = @Style;
        CUSTOM_ELEMENT = @Element;
        CUSTOM_VAR = @Var;
        TEMPLATE_STYLE = @Style;
        TEMPLATE_ELEMENT = @Element;
        TEMPLATE_VAR = @Var;
        ORIGIN_HTML = @Html;
        ORIGIN_STYLE = @Style;
        ORIGIN_JAVASCRIPT = @JavaScript;
        KEYWORD_ADD = add;
        KEYWORD_DELETE = delete;
        KEYWORD_INHERIT = inherit;
        KWYWORD_FROM = from;
        KEYWORD_AS = as;
        KEYWORD_TEXT = text;
        KEYWORD_STYLE = style;
        KEYWORD_CUSTOM = [Custom];
        KEYWORD_TEMPLATE = [Template];
        KEYWORD_ORIGIN = [Origin];
        KEYWORD_IMPORT = [Import]
        KEYWORD_NAMESPACE = [Namespace]

        // 组选项的数量限制，避免在大型项目中对性能的过高消耗
        OPTION_COUNT = 3;
    }
}
```

## 导入
你可以使用[Import]导入CHTL，HTML，CSS，JS文件 / 代码  

[Import] @Html from html文件路径 as(可选) 命名为  
[Import] @Style from css文件路径 as(可选) 命名为  
[Import] @JavaScript from js文件路径 as(可选) 命名为  
[Import] [Custom] @Element [需要导入的自定义元素名] from chtl文件路径 as(可选) 命名为  
[Import] [Custom] @Style [需要导入的样式组名称] from chtl文件路径 as(可选) 命名为  
[Import] [Custom] @Var [需要导入的变量组名称] from chtl文件路径 as(可选) 命名为  
[Import] [Template] @Element [需要导入的自定义元素名] from chtl文件路径 as(可选) 命名为  
[Import] [Template] @Style [需要导入的样式组名称] from chtl文件路径 as(可选) 命名为  
[Import] [Template] @Var [需要导入的变量组名称] from chtl文件路径 as(可选) 命名为  
[Import] @Chtl from chtl文件路径 / chtl文件名  
[Import] 上述任意一种 from 文件名(在当前目录下找)  

路径可以是标准相对路径，也可以使用.代替/  

### 通配导入
你可以使用/*来表示导入某一个文件夹中所有的文件  

```chtl
[Import] (如果不写) from ./module/*                  // 导入所有的html，css，js，chtl文件
```

## 命名空间
你可以使用[Namespace]创建命名空间，命名空间能够有效防止模块污染  
导入一整个文件，或导入了重名的任意单元时，命名空间起效  

xxx.chtl  
```chtl
[Namespace] space  // 如果文件没有命名空间，则默认使用文件名称来作为命名空间  

[Custom] @Element ys
{
    div
    {
        style
        {

        }
    }

    div
    {
        style
        {

        }
    }
}
```

```chtl
[Import] @Chtl from xxx

body
{
    @Element ys from space;  // 使用space命名空间中的ys自定义元素
}
```

### 命名空间嵌套
```chtl
[Namespace] space
{
    [Namespace] room  // 嵌套命名空间
    // 嵌套命名空间时，如果仅仅是只有一层关系 或 只有一层平级，可以不用写花括号

    [Custom] @Element ys
    {
        div
        {
            style
            {

            }
        }

        div
        {
            style
            {

            }
        }
    }
}
```

```chtl
[Namespace] space
{
    [Namespace] room
    {

    }

    [Namespace] room2
    {
        [Custom] @Element ys
        {
            div
            {
                style
                {

                }
            }

            div
            {
                style
                {

                }
            }
        }
    }
}
```

```chtl
[Import] @Chtl from xxx

body
{
    @Element ys from space.room2;  // 嵌套命名空间的使用
}
```

## 期盼
你可以使用execpt关键字来要求这个定义域内只能使用什么  

### 精准期盼
```
div
{
    execpt span, div, [Custom] @Element XXX, [Template] @Element XXX;  // 在这个定义域内，只能出现上述的内容，如果自定义元素和模板重名，则需要显性写出[Custom]和[Template]
}
```

### 类型期盼
```
div
{
    execpt [Custom];  // 只允许自定义，不允许模板
}
```

### 否定期盼
```
div
{
    not execpt [Template];  // 不允许出现模板
}
```

### 全局期盼
```
[Namespcae] xxx
{
    not execpt [Custom];

    [Template] // 错误，不允许定义模板
}
```