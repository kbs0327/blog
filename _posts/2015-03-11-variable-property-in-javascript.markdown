---
layout: article
title:  "자바스크립트에서의 변수 vs 프로퍼티"
date:   2015-03-11 12:20:13
image:
  teaser: programming.jpg
categories: technology
---

# 자바스크립트에서 변수와 프로퍼티의 차이
참고 글을 정리하여 만든 글입니다.  

## Variable Object
자바스크립트에서 코드는 Global context와 Function context 에서 실행됩니다.  
Global context는 전체 코드에서 1개만 생성이 되고 각각의 함수 호출시 마다 Function context가 생성됩니다.  
각각의 컨텍스트는 Variable Object를 가지는데 컨텍스트에서 선언된 변수들은 모두 Variable Object에 저장됩니다.  
이 Variable Object는 Rhino가 아니라면 접근할 수 없습니다.  

하지만, Global context는 특이하게 Variable Object를 Global Object에 가집니다.  

{% highlight js %}
var a = "hello";
console.log(window.a);  // hello

function func() {
  var b = "hi";
  console.log(b);       // hi
  console.log(func.b);  // undefined
  console.log(this.b);  // undefined
}
func();
{% endhighlight %}

## Property
ECMA5에서는 객체의 일부로 이름과 값사이의 관계라고 정의되어 있습니다.  

## Variable
ECMA5에서는 변수를 정의하지 않았습니다.  
그래서 참고 글에는 실행 컨텍스트 안에 있는 이름과 값 사이의 관계라고 정의했습니다.  

## 차이점

### 호이스팅
변수는 선언부가 호이스팅 되어 실행 컨텍스트 시작시에 만들어집니다.  
하지만 정의는 호이스팅 되지 않습니다.  
또한 프로퍼티 정의도 호이스팅 되지 않습니다.  

{% highlight js %}
alert(a); //undefined
alert(b); //ReferenceError: b is not defined
var a = 24;
window.b = 36;

// 실제 작동은 아래 처럼됩니다.
var a;    // 호이스팅

alert(a);
alert(b);
a = 24;
window.b = 36;
{% endhighlight %}

### attribute 초기화
변수는 delete를 못하는 옵션이 true이지만 프로퍼티는 false입니다.  

{% highlight js %}
//variable
var oneTimeInit = function() {
    //do stuff
}
delete oneTimeInit; //false (means it did not happen)
typeof oneTimeInit; "function";

//explicit property
window.oneTimeInit = function() {
    //do stuff
}
delete oneTimeInit; //true
typeof oneTimeInit; "undefined";
{% endhighlight %}

### illegal names
subscript 구문("[]")에서 property는 넣을 수 있지만 변수는 넣을 수 없습니다.  
{% highlight js %}
//illegal name
var a = "***";
window[a] = 123;
window[a]; //123 (Property lookup OK)
*** //ReferenceError (illegal name)

//legal name
var a = "foo";
window[a] = 123;
window[a]; //123
foo; //123

function c() {
  var b = "ddd";
  this[b] = 123;
  console.log(this[b]);     // 123
  console.log(ddd);         // error
  console.log(this["ddd"]); // 123
}
new c();

console.log(c["ddd"]);      // undefined
{% endhighlight %}
##참고자료  
[JavaScript, JavaScript… by Angus Croll](https://javascriptweblog.wordpress.com/2010/08/09/variables-vs-properties-in-javascript/)  
