---
layout: post
title:  "객체지향 프로그래밍"
date:   2015-03-05 20:52:13
image:
  teaser: javascript_book.jpg
categories: book
---

# 이 글은 책을 정리한 내용입니다.

ECMA-262는 객체를 '프로퍼티의 순서 없는 컬렉션이며 각 프로퍼티는 원시 값이나 객체, 메서드를 포함한다.'고 정의합니다.  
이 책의 저자는 자바스크립트의 객체를 해시 테이블에 비유해서 설명했으며 그게 가장 잘 설명한 것 같습니다.  

## 제약사항
ECMAScript 5판의 메서드는 인터넷 익스플로러 9 이상, 파이어폭스 4 이상, 사파리 5 이상, 오페라 12이상, 크롬에서 사용가능합니다.

## 객체에 대한 이해  

자바스크립트에서는 클래스라는 개념이 없습니다.  
그래서 function으로 객체를 만드는 특이한 방법을 사용합니다.  

{% highlight js %}
// name, age, job을 프로퍼티로 가지고 sayName이라는 메서드를 가지고 있는 객체
var person = new Object();
person.name = "Nicholas";
person.age = 29;
person.job = "Software Engineer";

person.sayName = function() {
  console.log(this.name);
};

// 위와 동일합니다.
var person = {
  name: "Nicholas",
  age: 29,
  job: "Software Engineer",

  sayName: function() {
    console.log(this.name);
  }
}
{% endhighlight %}  
이 예제에서의 메서드도 객체의 프로퍼티 중 일부입니다.  

### 프로퍼티 타입  
ECMA-262 5판에서는 프로퍼티의 특징을 내부적으로만 유효한 속성에 따라 설명합니다. 이를 기본 프로퍼티라고 부릅니다.  
기본 프로퍼티의 속성을 변경하려면 ECMAScript5판의 Object.defineProperty() 메서드를 사용해야합니다.  
이 메소드의 기본값은 false이므로 주의하여 사용해야 합니다.

{% highlight js %}
// 기본 프로퍼티 변경
var person = {};
Object.defineProperty(person, // 프로퍼티 추가 혹은 수정할 객체
  "name",                     // 프로퍼티 이름
  {                           // 서술자
  Writable: false,            // 사실상 [[Configurable]]도 false가 됨
  value: "Nicholas"
  });

  console.log(person.name); // Nicholas
  person.name = "Greg";
  console.log(person.name); // Nicholas
  // [[Writable]]이 false이면 무시됨. 단, strict모드에서는 에러

// [[Configurable]] 속성이 false일 때
var person = {};
Object.defineProperty(person, // 프로퍼티 추가 혹은 수정할 객체
  "name",                     // 프로퍼티 이름
  {                           // 서술자
  Configurable: false,
  value: "Nicholas"
  });

  console.log(person.name); // Nicholas
  delete person.name;
  console.log(person.name); // Nicholas
  // [[Configurable]]이 false이면 delete도 무시됨. 단, strict 모드에서는 에러.

// [[Configurable]]이 false이면 [[Writable]]만 수정가능.
var person = {};
Object.defineProperty(person, // 프로퍼티 추가 혹은 수정할 객체
  "name",                     // 프로퍼티 이름
  {                           // 서술자
  Configurable: false,
  value: "Nicholas"
  });

// 에러 발생
Object.defineProperty(person,  
  "name",
  {
  Configurable: true,
  value: "Nicholas"
  });
{% endhighlight %}  

#### 데이터 프로퍼티  

- [[Configurable]] - 프로퍼티가 delete로 삭제하거나 속성변경, 접근자 프로퍼티로 변환할 수 있음을 나타냅니다.  
- [[Enumerable]] - for-in 루프에서 해당 프로퍼티를 반환할 수 있음을 나타냅니다.  
- [[Writable]] - 프로퍼티의 값을 변경할 수 있음을 나타냅니다.
- [[Value]] - 프로퍼티의 실제 데이터 값이나 위치를 나타냅니다. 기본값은 undefined입니다.

#### 접근자 프로퍼티  

- [[Configurable]] - 위와 동일
- [[Enumberable]] - 위와 동일
- [[Get]] - 프로퍼티를 읽을 때 호출하는 메서드
- [[Set]] - 프로퍼티를 바꿀 때 호출하는 메서드

{% highlight js %}
//Object.defineProperty로 수정
var book = {
  _year: 2004, // _가 앞에 있으면 객체 메서드를 통해서만 접근할 것이라는 의도를 나타내는 표기법
  edition: 1
};

Object.defineProperty(book,
  "year",
  {
    get: function() {
      return this._year;
    },
    set: function(newValue) {
      this._year = newValue;
      this.edition += newValue - 2004;
    }
    });

book.year = 2005;
console.log(book.edition); // 2

// 5판 이전의 비표준 메서드 __defineGetter__(), __defineSetter__() 사용
var book = {
  _year: 2004,
  edition: 1
}

book.__defineGetter__("year", function() {
  return this._year;
  });

book.__defineSetter__("year", function(newValue) {
  this._year = newValue;
  this.edition += newValue - 2004;
  });

book.year = 2005;
console.log(book.edition); // 2
{% endhighlight %}
getter와 setter는 필수는 아닙니다. 만약 없다면 해당 메서드호출시에 무시되고 스트릭트 모드에서는 에러가 발생합니다.

다중 프로퍼티 지원 메서드로 Object.defineProperties()가 있습니다.  
또한 프로퍼티를 반환해주는 Object.getOwnPropertyDescriptor() 메서드가 있습니다. 이 메서드들도 ECMAScript 5판에서 정의되었습니다.  
{% highlight js %}
var book = {};

Object.defineProperties(book, {
  _year: {
    value: 2004
  },

  edition: {
    value: 1
  },

  year: {
    get: function() {
      return this._year;
    },
    set: function(newValue) {
      this._year = newValue;
      this.edition += newValue - 2004;
    }
  }
  });

var descriptor = Object.getOwnPropertyDescriptor(book, "_year");
console.log(descriptor.value);            // 2004
console.log(descriptor.configurable);     // false
console.log(typeof descriptor.get);       // undefined

var descriptor = Object.getOwnPropertyDescriptor(book, "year");
console.log(descriptor.value);            // undefined
console.log(descriptor.configurable);     // false
console.log(typeof descriptor.get);       // function
{% endhighlight %}

## 객체 생성

### 팩토리 패턴  
목적: Object생성시 중복된 코드를 줄이기  
방법: 객체 생성과정을 추상화

{% highlight js %}
function createPerson(name, age, job) {
  var o = new Object();
  o.name = name;
  o.age = age;
  o.job = job;
  o.sayName = function() {
    console.log(this.name);
  };
  return o;
}

var person1 = createPerson("Nicholas", 29, "Software Engineer");
var person2 = createPerson("Greg", 27, "Doctor");
{% endhighlight %}

문제점: 생성한 객체가 어떤 타입인지 알 수 없다는 문제점이 있음

### 생성자 패턴
목적: 위의 문제점을 해결  
방법: 커스텀 생성자를 만들어서 원하는 타입의 객체에 필요한 프로퍼티와 메서드 정의
{% highlight js %}
function Person(name, age, job) {
  this.name = name;
  this.age = age;
  this.job = job;
  this.sayName = function() {
    console.log(this.name);
  };
}

var person1 = new Person("Nicholas", 29, "Software Engineer");
var person2 = new Person("Greg", 27, "Doctor");

console.log(person1.constructor === Person); // true
console.log(person2.constructor === Person); // true

console.log(person1 instanceof Object); // true
console.log(person1 instanceof Person); // true
console.log(person2 instanceof Object); // true
console.log(person2 instanceof Person); // true

console.log(person1.sayName == person2.sayName); // false - 인스턴스마다 새 메서드 생성
{% endhighlight %}
Person메서드의 특징  

- 명시적으로 객체를 생성하지 않음  
- 프로퍼티와 메서드는 this 객체에 직접적으로 할당  
- return문이 없음  
- 메서드의 첫글자가 대문자(생성자 함수 표기법)  

new 연산자 호출시 순서도  

1. 객체를 생성  
2. 생성자의 this값에 새 객체를 할당, this는 새 객체를 가리킴  
3. 생성자 내부코드 실행  
4. 새 객체 반환  

생성자 함수와 다른함수의 차이는 new 연산자와 함께 호출하는가 입니다.  
{% highlight js %}
var person = new Person("Nicholas", 29, "Software Engineer");
person.sayName(); // Nicholas

Person("Greg", 27, "Doctor"); // window에 추가
window.sayName(); // Greg

var o = new Object();
Person.call(o, "Kristen", 25, "Nurse");
o.sayName(); // Kristen
{% endhighlight %}

문제점: 인스턴스마다 메서드가 생성됨
{% highlight js %}
function Person(name, age, job) {
  this.name = name;
  this.age = age;
  this.job = job;
  this.sayName = new Function("console.log(this.name)"); // 논리적으로 동등
}

// 우회방법
function Person(name, age, job) {
  this.name = name;
  this.age = age;
  this.job = job;
  this.sayName = sayName;
}

function sayName() {
  console.log(this.name);
}

var person1 = new Person("Nicholas", 29, "Software Engineer");
var person2 = new Person("Greg", 27, "Doctor");

console.log(person1.sayName == person2.sayName); // true
{% endhighlight %}
우회방법의 문제점: 일부 객체에서만 쓰는 함수를 전역에 놓음으로써 전역 스코프를 어지럽힘  

## 프로토타입 패턴
목적: 위의 문제점 해결  
방법: 모든 함수에서 가지고 있는 prototype 프로퍼티 사용

{% highlight js %}
function Person() {
}

Person.prototype.name = "Nicholas";
Person.prototype.age = 29;
Person.prototype.job = "Software Engineer";
Person.prototype.sayName = function() {
  console.log(this.name);
};

var person1 = new Person();
person1.sayName(); // Nicholas

var person2 = new Person();
person2.sayName(); // Nicholas

console.log(person1.sayName === person2.sayName); // true

console.log(Person.prototype.isPrototypeOf(person1)); // true
console.log(Person.prototype.isPrototypeOf(person2)); // true

// getPrototypeOf는 ECMAScript 5판 스펙
console.log(Object.getPrototypeOf(person1) == Person.prototype); // true
console.log(Object.getPrototypeOf(person1).name); // Nicholas
{% endhighlight %}

### 프로토타입 동작  

모든 프로토타입은 자동으로 constructor 프로퍼티를 가짐 - 소속된 함수를 가리킴  
커스텀 생성자를 호출하여 인스턴스를 만들면 인스턴스 내부에 [[prototype]] 포인터가 생성됩니다.  
[[prototype]]에 접근하는 표준은 없지만 파이어폭스, 사파리, 크롬에서는 __proto__라는 프로퍼티를 지원합니다.  
검색을 할 때 객체 인스턴스에서 찾지못하면 프로토타입에서 검색합니다.  
즉, 생성자로 만들어진 객체들은 같은 prototype을 공유하고, 프로퍼티와 메서드를 공유합니다.  

소개되는 메서드들  
hasOwnProperty: 프로퍼티가 어디에 존재하는지 확인  
in 연산자: for in구문이 아닌 자체 in이 사용되면 인스턴스에 존재하든 프로토타입에 존재하든 모두 true를 반환합니다.  
- in 연산자의 경우 스코프 체인을 따라가면서 프로퍼티가 존재하는지 확인  
- 인스턴스에서 프로토타입에 [[Enumerable]] 속성이 false인 프로퍼티를 덮어쓰면 in 루프에서 찾을 수 있습니다.(IE8 이전에서는 버그)  
hasPrototype: 책에서 소개된 이 메서드는 object의 인스턴스 혹은 메서드의 caller에 따라서 다르게 동작할 수 있습니다.(일반적이지 않음)  

{% highlight js %}
function Person() {
}

Person.prototype.name = "Nicholas";
Person.prototype.age = 29;
Person.prototype.job = "Software Engineer";
Person.prototype.sayName = function() {
  console.log(this.name);
};

var person1 = new Person();
var person2 = new Person();

console.log(person1.hasOwnProperty("name")); // false
console.log("name" in person1); // true
console.log("toString" in person1); // true

person1.name = "Greg";
console.log(person1.name); // Greg - 인스턴스에서
console.log(person1.hasOwnProperty("name")); // true
//console.log(person1.prototype.hasOwnProperty("name")); // 에러발생
a
console.log("name" in person1); // true

console.log(person2.name); // Nicholas - 프로토타입에서
console.log(person2.hasOwnProperty("name")); // false
console.log("name" in person2); // true

delete person1.name;
console.log(person1.name); // Nicholas - 프로토타입에서 검색
console.log(person1.hasOwnProperty("name")); // false
console.log("name" in person1); // true
{% endhighlight %}

for in 구문  
{% highlight js %}
var o = {
  toString : function() {
    return "My Object";
  }
};

for (var prop in o) {
  if (prop == "toString") {
    console.log("Found toString");
  }
}
{% endhighlight %}

ECMAScript 5판의 메서드인 Object.keys() 메서드를 사용하면 객체 인스턴스에서 나열가능한 프로퍼티 전체 목록을 얻을 수 있습니다.  
또한 ECMAScript 5판의 메서드인 [[Enumerable]] 속성 상관없이 목록을 얻으려면 Object.getOwnPropertyNames()메서드를 사용하면 됩니다.

{% highlight js %}
function Person() {
}

Person.prototype.name = "Nicholas";
Person.prototype.age = 29;
Person.prototype.job = "Software Engineer";
Person.prototype.sayName = function() {
  console.log(this.name);
};

var keys = Object.keys(Person.prototype);
console.log(keys); // name,age,job,sayName

var p1 = new Person();
p1.name = "Rob";
p1.age = 31;
var p1keys = Object.keys(p1);
console.log(p1keys); // name, age

var keys = Object.getOwnPropertyNames(Person.prototype);
console.log(keys); // constructor,name,age,job,sayName
{% endhighlight %}

#### 프로토타입의 대체 문법

{% highlight js %}
function Person() {
}
/*
Person.prototype = {
  name: "Nicholas",
  age: 29,
  job: "Software Engineer",
  sayName: function() {
    console.log(this.name);
  }
};
// constructor가 사라지는 문제발생

var friend = new Person();
console.log(friend instanceof Object); // true
console.log(friend instanceof Person); // true
console.log(friend.constructor == Person); // false
console.log(friend.constructor == Object); // true
*/

Person.prototype = {
  constructor: Person, // [[Enumerable]] 속성이 true가 됨 - 이걸 해결하려면 ECMAScript5판의 defineProperty로 해결가능
  name: "Nicholas",
  age: 29,
  job: "Software Engineer",
  sayName: function() {
    console.log(this.name);
  }
};

var friend = new Person();
console.log(friend instanceof Object); // true
console.log(friend instanceof Person); // true
console.log(friend.constructor == Person); // true
console.log(friend.constructor == Object); // false
{% endhighlight %}

#### 프로토타입의 동적성질
프로토타입의 값을 찾는것은 런타임시이므로 프로토타입을 변경하면 그 변경은 바로 반영됩니다.
인스턴스가 생성될 때 생성자에서 프로토타입을 가리키는 [[Prototype]] 포인터가 할당됩니다.  

{% highlight js %}
function Person() {
}

var friend = new Person();

Person.prototype.sayHi = function() {
  console.log("hi");
};

friend.sayHi(); // hi동작함

Person.prototype = {
  constructor: Person,
  name: "Nicholas",
  age: 29,
  job: "Software Engineer",
  sayName: function() {
    console.log(this.name);
  }
}

friend.sayName(); // 에러
{% endhighlight %}

#### 네이티브 객체 프로토타입  
네이티브 참조 타입도 프로토타입 패턴으로 구현되었습니다.  
그래서 네이티브 참조 타입도 새 메서드를 추가, 수정할 수 있습니다.  
하지만 배포하는 코드에서는 가급적 피하길 권장합니다. (충돌, 기본메서드 덮어쓰기)  

{% highlight js %}
console.log(typeof Array.prototype.sort); // function
console.log(typeof String.prototype.substring); // function

String.prototype.startsWith = function (text) {
  return this.indexOf(text) == 0;
}

var msg = "Hello world!";
console.log(msg.startsWith("Hello"));
{% endhighlight %}

#### 프로토타입의 문제점
공유라는 성질이 주요 문제점입니다.

{% highlight js %}
function Person() {
}

Person.prototype = {
  constructor: Person,
  name: "Nicholas",
  age: 29,
  job: "Software Engineer",
  friends: ["Shelby", "Court"],
  sayName: function() {
    console.log(this.name);
  }
};

var person1 = new Person();
var person2 = new Person();

person1.friends.push("Van");

console.log(person1.friends); // Shelby,Court,Van
console.log(person2.friends); // Shelby,Court,Van
console.log(person1.friends === person2.friends); // true
{% endhighlight %}

### 생성자 패턴과 프로토타임 패턴의 조합  
목적: 프로토타입의 공유문제점 해결  
방법: 프로토타입에는 공유하는 프로퍼티만 넣고 그 외는 생성자에 넣습니다.  

{% highlight js %}
function Person() {
  this.name = name;
  this.age = age;
  this.job = job;
  this.friends = ["Shelby", "Court"];
  /*
  //동적 프로토타입 패턴
  if (typeof this.sayName != "function") {
    Person.prototype.sayName = function() {
      console.log(this.name);
    };
  }
  */
}

Person.prototype = {
  constructor: Person,
  sayName: function() {
    console.log(this.name);
  }
};

var person1 = new Person("Nicholas", 29, "Software Engineer");
var person2 = new Person("Greg", 27, "Doctor");

person1.friends.push("Van");

console.log(person1.friends); // Shelby,Court,Van
console.log(person2.friends); // Shelby,Court
console.log(person1.friends === person2.friends); // false
console.log(person1.sayName === person2.sayName); // true
{% endhighlight %}

### 기생 생성자 패턴  
목적: 다른 패턴이 실패할 때 폴백으로 사용하는 패턴  
방법: Wrapper function을 만듬

{% highlight js %}
function Person(name, age, job) {
  var o = new Object();
  o.name = name;
  o.job = job;
  o.sayName = function() {
    console.log(this.name);
  };
  return o;
}

var friend = new Person("Nicholas", 29, "Software Engineer");
friend.sayName(); // Nicholas
{% endhighlight %}

추가: 객체생성자를 만들 수 있습니다.

{% highlight js %}
function SpecialArray() {
  var values = new Array();

  values.push.apply(values, arguments);

  values.toPipedString = function() {
    return this.join("|");
  };

  return values;
}

var colors = new SpecialArray("red", "blue", "green");
console.log(colors.toPipedString()); // red|blue|green
{% endhighlight %}

### 방탄 생성자 패턴
목적: 공용프로퍼티가 없고 메서드가 this를 참조하지 않는 객체(durable 객체)를 만드는 생성자  
방법: 클로저를 사용하며 생성자를 호출시에 new로 호출하지 않습니다.

{% highlight js %}
function Person(name, age, job) {
  var o = new Object();

  o.sayName = function() {
    console.log(name);
  };

  return o;
}

var friend = Person("Nicholas", 29, "Software Engineer");
friend.sayName(); // Nicholas
{% endhighlight %}

## 상속

### 프로토타입 체인
목적: 객체를 상속하기 위해 사용  
방법: prototype에 supertype의 객체를 생성

{% highlight js %}
function SuperType() {
  this.property = true;
}

SuperType.prototype.getSuperValue = function() {
  return this.property;
};

function SubType() {
  this.subproperty = false;
}

SubType.prototype = new SuperType();
SubType.prototype.getSubValue = function() {
  return this.subproperty;
};

var instance = new SubType();
console.log(instance.getSuperValue()); // true

console.log(instance instanceof Object); // true
console.log(instance instanceof SuperType); // true
console.log(instance instanceof SubType); // true

console.log(Object.prototype.isPrototypeOf(instance)); // true
console.log(SuperType.prototype.isPrototypeOf(instance)); // true
console.log(SubType.prototype.isPrototypeOf(instance)); // true

// 기존 메서드를 오버라이드
SubType.prototype.getSuperValue = function () {
  return false;
};


console.log(instance.getSuperValue()); // false

SubType.prototype = {
  getSubValue: function() {
    return this.subproperty;
  },
  someOtherMethod: function() {
    return false;
  }
};

instance = new SubType();
console.log(instance.getSuperValue()); // error
{% endhighlight %}

문제점: 프로토타입 프로퍼티는 모든 객체에서 공유함
{% highlight js %}
function SuperType() {
  this.colors = ["red", "blue", "green"];
}

function SubType() {
}

SubType.prototype = new SuperType();

var instance1 = new SubType();
instance1.colors.push("black");
console.log(instance1.colors); // red,blue,green,black

var instance2 = new SubType();
console.log(instance2.colors); // red,blue,green,black
{% endhighlight %}

### 생성자 훔치기
목적: 위의 문제점을 해결함  
방법: 생성자를 SubType생성자에서 호출함(this변경)

{% highlight js %}
/*
function SuperType() {
  this.colors = ["red", "blue", "green"];
}

function SubType() {
  SuperType.call(this);
}

var instance1 = new SubType();
instance1.colors.push("black");
console.log(instance1.colors); // red,blue,green,black

var instance2 = new SubType();
console.log(instance2.colors); // red,blue,green
*/
// 매개변수도 넘길 수 있음
function SuperType(name) {
  this.name = name;
}

function SubType() {
  SuperType.call(this, "Nicholas");

  this.age = 29;
}

var instance = new SubType();
console.log(instance.name); // Nicholas
console.log(instance.age); // 29
{% endhighlight %}

문제: 함수 재사용이 불가능해짐

### 조합상속
목적: 함수도 재사용함  
방법: 생성자 훔치기 패턴과 프로토타입 체인 패턴의 장점 취함
{% highlight js %}
function SuperType(name) {
  this.name = name;
  this.colors = ["red", "blue", "green"];
}

SuperType.prototype.sayName = function() {
  console.log(this.name);
};

function SubType(name, age) {
  SuperType.call(this, name);

  this.age = age;
}

SubType.prototype = new SuperType();

SubType.prototype.sayAge = function() {
  console.log(this.age);
};

var instance1 = new SubType("Nicholas", 29);
instance1.colors.push("black");
console.log(instance1.colors); // red,blue,green,black
instance1.sayName(); // Nicholas
instance1.sayAge(); // 29

var instance2 = new SubType("Greg", 27);
console.log(instance2.colors); // red,blue,green
instance2.sayName(); // Greg
instance2.sayAge(); // 27
{% endhighlight %}

### 프로토타입 상속

더글러스 크록포드가 소개한 엄격히 정의된 생성자를 쓰지 않고도 상속 구현하는 방법

{% highlight js %}
function object(o) {
  function F() {}
  F.prototype = o;
  return new F();
}

//위의 함수를 사용하여 상속 구현
var person = {
  name: "Nicholas",
  friends: ["Shelby", "Court", "Van"]
};

var anotherPerson = Object(person);
anotherPerson.name = "Greg";
anotherPerson.friends.push("Rob");

var yetAnotherPerson = object(person);
yetAnotherPerson.name = "Linda";
yetAnotherPerson.friends.push("Barbie");

console.log(person.friends); // Shelby,Court,Van,Rob,Barbie

// ECMAScript 5판에는 Object.create()메서드를 추가했습니다.
// create 메서드의 두번째 매개변수는 defineProperties와 같은 형식입니다.
var person = {
  name: "Nicholas",
  friends: ["Shelby", "Court", "Van"]
};

var anotherPerson = Object.create(person);
anotherPerson.name = "Greg";
anotherPerson.friends.push("Rob");

var yetAnotherPerson = Object.create(person);
yetAnotherPerson.name = "Linda";
yetAnotherPerson.friends.push("Barbie");

console.log(person.friends); // Shelby,Court,Van,Rob,Barbie
{% endhighlight %}

### 기생상속
목적: 더글라스 크락포드가 만든 개념으로 상속을 담당할 함수를 만들고 객체를 확장해서 반환함

{% highlight js %}
function createAnother(original) {
  var clone = object(original);
  clone.sayHi = function() {
    console.log("hi");
  };
  return clone;
}

// 사용
var person = {
  name: "Nicholas",
  friends: ["Shelby", "Court", "Van"]
};
var anotherPerson = createAnother(person);
anotherPerson.sayHi(); // hi
{% endhighlight %}

### 기생 조합 상속
조합 상속은 자주 쓰이는 상속 패턴이지만 비효율적이게 상위 타입생성자가 2번 호출됩니다.
이를 고치는 방법으로 기생 조합 상속이 나왔습니다.

{% highlight js %}
function SuperType(name) {
  this.name = name;
  this.colors = ["red", "blue", "green"];
}

SuperType.prototype.sayName = function() {
  console.log(this.name);
};

function SubType(name, age) {
  SuperType.call(this, name);                 // SuperType 생성자 호출

  this.age = age;
}

SubType.prototype = new SuperType();          // SuperType 생성자 호출
SubType.prototype.constructor = Subtype;
SubType.prototype.sayAge = function() {
  console.log(this.age);
};

// 위를 수정하기 위한 함수
function inheritPrototype(subType, superType) {
  var prototype = object(superType.prototype);   // 객체 생성
  prototype.constructor = subType;               // 객체 확장
  subType.prototype = prototype;                 // 객체 할당
}

// 적용
function SuperType(name) {
  this.name = name;
  this.colors = ["red", "blue", "green"];
}

SuperType.prototype.sayName = function() {
  console.log(this.name);
};

function SubType(name, age) {
  SuperType.call(this, name);

  this.age = age;
}

inheritPrototype(SubType, SuperType);

SubType.prototype.sayAge = function() {
  console.log(this.age);
};
{% endhighlight %}

##참고자료  
Nicholas C. Zakas. (2013). 프론트엔드 개발자를 위한 자바스크립트 프로그래밍, (한선용 옮김). 인사이트
