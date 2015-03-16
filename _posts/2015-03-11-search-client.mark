---
layout: article
title:  "클라이언트 탐지"
date:   2015-03-11 16:25:13
image:
  teaser: javascript_book.jpg
categories: book
---

# 클라이언트 탐지
브라우저마다 다르게 작동하거나 혹은 메소드가 없는 경우가 많이 있습니다.  
그래서 브라우저별로 우회해서 개발을 하곤합니다.  
하지만 클라이언트 탐지는 반드시 마지막 수단으로 미뤄야 합니다.  

## 기능탐지
브라우저는 관심이 없고 기능을 제공하는지에 관심을 가집니다.  
보통 아래와 같은 방법으로 구현합니다.  
{% highlight js %}
if (object.propertyInQuestion) {
  // object.propertyInQuestion 사용
}

// 실제 사용 예
function getElement(id) {
  if (document.getElementById(id)) {
    return document.getElementById(id);
  } else if (document.all) {
    return document.all[id];
  } else {
    throw new Error("No way to retrieve element!");
  }
}
{% endhighlight %}
가장 일반적인 방법을 먼저 테스트 합니다.  
그리고 어떤 기능이 있다고 해서 그 브라우저라고 확신하면 안됩니다.  
{% highlight js %}
function getWindowWidth() {
  if (document.all) {
    return document.documentElement.clientWidth; // 잘못된 사용
  } else {
    return window.innerWidth;
  }
}
{% endhighlight %}

### 안전한 기능 탐지  

단순히 존재 여부만 확인하는 것은 잘못된 방법입니다.
{% highlight js %}
function isSortable(object) {
  return !!object.sort;
}

// 개선 방법
function isSortable(object) {
  return typeof object.sort == "function";
}

// IE8 이하에서 제대로 동작 안하는 코드
function hasCreateElement() {
  return typeof document.createElement == "function";
}
{% endhighlight %}
하지만 typeof 연산자가 만능은 아닙니다.  
인터넷 익스플로러 8 이전버전은 JScript 대신 COM을 써서 호스트 객체를 구현했습니다.  
그래서 function이 아닌 object를 반환할 수도 있습니다.

### 기능탐지와 브라우저 탐지는 다릅니다.
기능탐지를 브라우저 탐지처럼 사용하면 안됩니다.  
{% highlight js %}
// 이렇게 하면 안됩니다. 이것은 기능탐지일 뿐입니다.
var isFirefox = !!(navigator.vendor && navigator.vendorSub);
// 이렇게 하면 안됩니다. 너무 많은 것을 가정합니다.
var isIE = !!(document.all && document.uniqueID);

// 몇가지 기능을 묶어서 브라우저 그룹으로 만들기는 적절
var hasNSPlugins = !!(navigator.plugins && navigator.plugins.length);

// DOM1지원 체크
var hasDOM1 = !!(document.getElementById && document.createElement && document.getElementsByTagName);
{% endhighlight %}

### 쿽스 탐지
쿽스탐지는 정확하게 동작하지 않는 것을 탐지하는 것입니다.  

인터넷 익스플로러8과 이전 버전에서는 [[Enummerable]] 속성이 false이면  
같은 이름의 프로토타입 프로퍼티를 for-in 루프에 표시하지 않는 버그가 있습니다.  
이를 탐지하는 코드입니다.
{% highlight js %}
hasDontEnumQuirk = function() {
  var o = {toString : function() {}};
  for (var prop in o ) {
    if (prop == "toString") {
      return false;
    }
  }

  return true;
}();
{% endhighlight %}

사파리 3 미만에서는 가려진 프로퍼티를 나열하는 버그가 있습니다.  
이를 탐지하는 코드입니다.
{% highlight js %}
hasDontEnumShadowQuirk = function() {
  var o = {toString : function() {}};
  var count = 0;
  for (var prop in o ) {
    if (prop == "toString") {
      count++;
    }
  }

  return (count > 1);
}();
{% endhighlight %}

## 브라우저 탐지
보통은 userAgent를 보고 어떤 브라우저인지 확인합니다.
하지만 이는 최후의 수단으로 생각해야합니다.  
userAgent는 착각을 유도하는 정보가 많이 들어가 있는 '위장'의 역사였습니다.  

### 역사
HTTP 1.1 agent 명세
> 제품 토큰은 통신 애플리케이션이 소프트웨어 이름과 버전을 통해 자신을 식별하도록 한다.  
> 제품 토큰을 사용하는 필드 대부분은 제품을 식별하는데 도움이 될, 공백으로 구분된 부수적 토큰도 받을 수 있다.  
> 제품(이름)은 해당 애플리케이션을 식별하기 쉬운 구절부터 나열한다.  

##### 초기 브라우저
초기 브라우저는 1923년에 NCSA(국립 슈퍼컴퓨터 애플리케이션 센터)에서 만들어졌습니다.
모자이크 userAgent: Mosaic/0.9


1. Initializes a new native object
2. Sets the internal [[Prototype]] of this object, pointing to the Function prototype property.
    If the function's prototype property is not an object (a primitive values, such as a Number, String, Boolean, Undefined or Null), Object.prototype is used instead.
3. After creating the object, it calls the function, providing the object as its this value.
4. If the return value of the called function, is a primitive, the object created internally is returned.
5. Otherwise, if an object is returned, the object created internally is lost.

Person() {
  this.aaa = 3;
}

var p1 = new Person();

var obj = {};
[this] = obj;
Person(); // this 포인터가
return [this]; // obj



Person() {
  this.aaa = 3;
}

var p1 = new Person();

var obj = {};
obj.prototype = Person.prototype;
[this] = obj;
var r = Person.call(obj);
if r is Object
  return r;
else
  return obj;

##참고자료  
Nicholas C. Zakas. (2013). 프론트엔드 개발자를 위한 자바스크립트 프로그래밍, (한선용 옮김). 인사이트  
[Insanehong 블로그](http://insanehong.Pkr/post/javascript-scope/)  
