---
layout: post
title:  "함수 표현식"
date:   2015-03-09 11:25:13
image:
  teaser: javascript_book.jpg
categories: book
---

# 함수 표현식

함수를 정의하는 방법은 함수 선언과 함수 표현식 두가지가 있습니다.  

## 함수선언문
함수 선언문을 사용할 때 비표준 프로퍼티인 name을 사용하면 함수 이름을 알 수 있습니다.(IE는 지원 안함)  
함수 선언문은 함수 호이스팅이 일어남 - 함수 선언 전에 함수 호출해도 정상작동(함수 선언을 먼저 실행시킴)  

{% highlight js %}
function functionName(arg0, arg1, arg2) {
  // 함수 본문
}
{% endhighlight %}

## 함수표현식
함수의 이름이 없어서 익명(람다)함수라고 부릅니다. - name 프로퍼티는 빈 문자열입니다.  
따라서 함수 선언전에 호출하면 에러가 발생합니다.  

{% highlight js %}
var functionName = function(arg0, arg1, arg2) {
  // 함수 본문
}
{% endhighlight %}

함수 선언문을 condition으로 정의하면 브라우저별로 다르게 동작할 수 있습니다.(사용 금지)  
하지만 함수 정의문을 condition으로 정의하는 것은 괜찮습니다.  
{% highlight js %}
// 쓰면 안됨
if(condition) {
  function sayHi() {
    alert("Hi!");
  }
} else {
  function sayHi() {
    alert("Yo!");
  }
}

// 이 예제는 사용해도 됩니다.
var sayHi;
if(condition) {
  sayHi = function() {
    alert("Hi");
  };
} else {
  sayHi = function() {
    alert("Yo!");
  };
}
{% endhighlight %}

## 재귀 함수
함수가 자기 자신을 이름으로 호출하는 형태
{% highlight js %}
function factorial(num) {
  if(num <= 1) {
    return 1;
  } else {
    return num * factorial(num-1);
  }
}

// 이 함수는 동작하지 않습니다.
var anotherFactorial = factorial;
factorial = null;
alert(anotherFactorial(4)); // 에러 - anotherFactorial 함수에서 factorial 함수를 호출하나 factorial 함수는 null
{% endhighlight %}  
이러한 경우 스트릭트 모드가 아닐 때에는 arguments.callee를 사용하면 에러가 발생하지 않습니다.  
그리고 다른 방식으로 이름 붙은 함수 표현식을 사용하면 스트릭트 모드에서도 에러가 발생하지 않습니다.  
{% highlight js %}
// arguments.callee 사용
function factorial(num) {
  if(num <= 1) {
    return 1;
  } else {
    return num * arguments.callee(num-1);
  }
}

// 이름 붙은 함수 표현식 사용
var factorial = (function f(num) {
  if (num <= 1) {
    return 1;
  } else {
    return num * f(num-1);
  }
});
{% endhighlight %}

## 클로저
다른 함수의 스코프에 있는 변수에 접근 가능한 함수

##### 스코프 체인
함수를 호출하면 실행 컨텍스트와 스코프 체인이 생성됩니다.  
함수를 실행하면 값을 읽거나 쓸 변수를 스코프 체인에서 검색합니다.  

{% highlight js %}
function compare(value1, value2) {
  if (value1 < value2) {
    retutrn -1;
  } else if (value1 > value2) {
    return 1;
  } else {
    return 0;
  }
}

var result = compare(5, 10);
{% endhighlight %}
compare함수를 정의하면 스코프 체인이 생성되고 이를 함수 내부의 [[Scope]] 프로퍼티에 저장합니다.  
함수를 호출하면 [[Scope]]프로퍼티에 들어있는 객체를 복사하여 스코프 체인이 생성됩니다.  
함수 실행이 끝나면 로컬 활성화 객체는 파괴됩니다.  

하지만 클로저에서는 다르게 작동됩니다.  
외부함수가 실행을 마치고 익명함수를 반환하면, 익명함수의 스코프 체인은 외부함수의 활성화 객체와 전역변수 객체를 포함하게 됩니다.  
외부함수가 실행을 마쳤는데도 활성화 객체는 파괴되지 않고, 스코프 체인만 파괴됩니다.  
활성화 객체는 익명함수가 파괴될 때까지 메모리에 남습니다.  

이해를 돕기 위해서 다이어그램을 추가하겠습니다.(출처: Insanehong 블로그)  
<img src="http://insanehong.kr/post/javascript-scope/@img/diagram3.jpeg">  

### 클로저와 변수
클로저는 항상 외부함수의 변수의 최종 값만 알 수 있습니다.
{% highlight js %}
function createFunctions() {
  var result = new Array();

  for (var i = 0; i < 10; i++) {
    result[i] = function() {
      return i;   // 모든 함수가 10을 반환함 - 참조만 하고 있으므로
    };
  }

  return result;
}
{% endhighlight %}

### this 객체
런타입에서 함수가 실행중인 컨텍스트에 묶입니다.
{% highlight js %}
var name = "The Window";

var object = {
  name: "My Object",

  getNameFunc: function() {
    // var that = this;
    return function() {
      return this.name;
      // return that.name;
    };
  }
};

alert(object.getNameFunc()()); // The Window
// 주석을 실행 시키면 My Object라고 출력됨
{% endhighlight %}
모든 함수는 호출되는 순간 자동으로 this와 arguments 두 특별한 변수를 가지게 됩니다.  
위의 예처럼 this객체를 that으로 따로 저장하면 this를 유지시킬 수 있습니다.  
{% highlight js %}
var name = "The Window";

var object = {
  name: "My Object",

  getName: function() {
    return this.name;
  }
};

alert(object.getName()); // My Object
alert((object.getName)()); // My Object
alert((object.getName = object.getName)()); // The Window
{% endhighlight %}
object의 getName을 호출하면 this는 object 객체가 되고  
object.getName은 (object.getName)은 같은 것으로 정의되어 있어서 this는 유지됩니다.  
하지만 먼저 할당 후에 그 결과를 호출햇으므로 this값은 유지되지 않아서 The Window를 반환합니다.  

### 메모리 누수
클로저는 인터넷 익스플로러 9이전 버전에서 메모리 문제르 일으킵니다.
JScript와 COM의 가비지 컬렉션방법이 다르기 때문입니다.

{% highlight js %}
function assignHandler() {
  var element = document.getElementById("someElement");
  var id = element.id; // id 사본을 저장

  element.onclick = function() {
    alert(id);
  };

  element = null; // 메모리 해제를 위해 null할당
}
{% endhighlight %}
클로저가 element를 직접 참조하는 건 아니지만 외부함수 활성화 객체에 저장되있으므로 null로 해제해야합니다.

## 블록 스코프 흉내내기
자바스크립트는 블록 스코프가 존재하지 않습니다. 그리고 변수를 2번 선언하더라도 에러를 내지 않습니다.
{% highlight js %}
function outputNumbers(count) {
  for (var i = 0; i < count; i++) {
    alert(i);
  }

  var i; // 변수 재선언
  alert(i); // count
}
{% endhighlight %}

익명함수를 통해 블록스코프를 흉내낼 수 있습니다.(고유 스코프)
{% highlight js %}
(function() {

  })();
{% endhighlight %}
익명 함수를 정의하는 즉시 호출합니다.  
괄호로 감싸는 이유는 자바스크립트에서 function 키워드를 함수 선언의 시작으로 인식하는데,  
함수 선언 다음에 괄호를 쓸 수 없습니다.  

## 고유 변수
자바스크립트는 private member는 존재하지 않습니다. 하지만 privilged 메서드를 통해 흉내낼 수 있습니다.  

메소드를 클로저로 만들어서 private member를 만드는 방법입니다.  
{% highlight js %}
function MyObject() {
  var privateVariable = 10;

  function privateFunction() {
    return false;
  }

  this.publicMethod = function() {
    privateVariable++;
    return privateFunction();
  };
}
{% endhighlight %}
이를 응용하여 특정 멤버를 정의해서 직접적으로 수정이 불가능하게 보호할 수 있습니다.
{% highlight js %}
function Person(name) {
  this.getName = function() {
    return name;
  };

  this.setName = function (value) {
    name = value;
  };
}

var person = new Person("Nicholas");
alert(person.getName()); // Nicholas
person.setName("Greg");
alert(person.getName()); // Greg
{% endhighlight %}
이 패턴의 문제점은 오직 생성자 패턴을 통해서만 이런 결과가 가능하다는 것입니다.

### 정적 고유 변수
고유 스코프를 통해서도 고유변수를 정의할 수 있습니다.
{% highlight js %}
(function() {
  var privateVariable = 10;

  function privateFunction() {
    return false;
  }

  MyObject = function() {
  }

  MyObject.prototype.publicMethod = function() {
    privateVariable++;
    return privateFunction();
  };
})();
{% endhighlight %}
MyObject에 var를 사용하지 않아서 고유 스코프가 아닌 전역에 위치합니다.

이를 응용하면 아래와 같이 사용할 수 있습니다.
{% highlight js %}
(function() {
  var name = "";
  Person = function(value) {
    name = value;
  };

  Person.prototype.getName = function() {
    return name;
  };

  Person.prototype.setName = function(value) {
    name = value;
  }
})();

var person1 = new Person("Nicholas");
alert(person1.getName()); // Nicholas
person1.setName("Greg");
alert(person1.getName()); // Greg

var person2 = new Person("Michael");
alert(person1.getName()); // Michael
alert(person2.getName()); // Michael
{% endhighlight %}
인스턴스가 독립 변수를 가질 수 없지만 프로토타입을 통해 코드 재사용성은 좋아집니다.

### 모듈 패턴
더글러스 크록포드가 고안한 모듈 패턴은 싱글톤 같은 일을 합니다.

{% highlight js %}
var singleton = {
  name: value,
  method: function() {
    // 메서드 코드
  }
};
{% endhighlight %}
모듈 패턴은 기본 싱글톤을 확장해서 고유변수와 특권 메서드를 쓸 수 있습니다.
{% highlight js %}
var singleton = function() {
  var privateVariable = 10;

  function privateFunction() {
    return false;
  }

  return {
    publicProperty: true,

    publicMethod: function() {
      privateVariable++;
      return privateFunction();
    }
  };
}();
{% endhighlight %}
반환된 객체 리터럴에는 공용이 될 프로퍼티와 메서드만 들어있습니다.  
모듈 패턴은 하나의 객체를 반드시 생성하고 몇가지 데이터를 가지며 고유 데이터에 접근 가능한 고유 메서드를 외부에 노출하도록 초기화 할 때 유용합니다.  

### 모듈 확장 패턴
이 패턴은 모듈패턴을 확장할 수 있게 하는 패턴입니다.
{% highlight js %}
var application = function() {
  var components = new Array();

  components.push(new BaseComponent());

  var app = new BaseComponent();

  app.getComponentCount = function() {
    return components.length;
  };

  app.registerComponent = function(component) {
    if(typeof component == "object") {
      components.push(component);
    }
  };

  return app;
}();
{% endhighlight %}

##참고자료  
Nicholas C. Zakas. (2013). 프론트엔드 개발자를 위한 자바스크립트 프로그래밍, (한선용 옮김). 인사이트  
[Insanehong 블로그](http://insanehong.kr/post/javascript-scope/)  
