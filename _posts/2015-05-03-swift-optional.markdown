---
layout: article
title: "swift의 optional"
date: 2015-05-03 14:07:13
image:
  teaser: swift.png
categories: technology
---

# swift의 optional
swift에서는 optional이라는 특이한 개념이 도입됬습니다.
이 개념은 제가 앱을 만들 때 계속 컴파일 에러를 일으키는 개념이어서 한번 정리해보았습니다.

## swift란
먼저 swift라는 언어는 ios 앱을 개발하는데 사용되는 언어로 기존의 object-C를 대체하기 위해 애플사에서 내놓은 언어입니다.

## optional이란
swift의 문법중에 특이한 것으로 말 그대로 값이 있어도 되고 없어도(nil)된다고 선언하는 것입니다.
이 optional은 앱을 만들 때에 컴파일 에러를 많이 일으키는데 그 이유는 swift가 type safety한 언어인데다가,
nil(null값)을 받지 못하는 변수가 기본이기 때문입니다.

이 때문에 보통 변수에 optional값을 대입하면 컴파일 에러가 발생합니다.

예를 들어서 다음과 같은 대입하는 코드가 있다고 합니다.
{% highlight swift %}
name = myName
{% endhighlight %}
여기서 name이 optional 값이 아닌데 optional인 myName이 대입되면 에러가 발생합니다.

## 문법
optional을 표현하는 문법은 변수를 할당할 때에 ?를 붙이면 됩니다.
{% highlight swift %}
let name: String?
{% endhighlight %}

## optional 컴파일 에러
optional로 인해서 컴파일 에러가 나는 경우는 함수에서 발생합니다.
함수에서 매개변수로 넘어온 값은 optional인지 아닌지 모릅니다. 그래서 매개변수의 값을 명확하게 정의해야합니다.
{% highlight swift %}
func myFunc(name: String!, age: Int?) {
  self.name = name
  self.age = age
}
{% endhighlight %}
위의 예에서 !를 붙이는 것은 optional이 아니라는 것을 명시해 주는 것입니다.

## casting
optional 값으로 정의를 했다고 해도 optional이 아닌 nil이 아닌게 명확하다면 대입할 수 있습니다.
바로 casting을 통하여 대입할 수 있는데 casting 방법은 다음과 같습니다.
{% highlight swift %}
name = myName!
{% endhighlight %}
이렇게 casting을 하여 값을 대입할 수도 있고 메소드를 호출할 수도 있습니다.

## optional과 if
optional 값을 사용할 때에 보통 nil체크를 가장 많이 합니다.
그래서 swift에서는 nil체크에 대한 optional 변수만의 특별한 방법을 제시합니다.
{% highlight swift %}
if let value = optionalValue {
  // optionalValue가 값이 있을 때 실행
} else {
  // optionalValue가 nil일 때 실행
}
{% endhighlight %}
cf) swift에서는 if문 같은 Bool값이 들어가는 부분에 Bool이 아닌 다른 변수를 넣을 수가 없습니다.

이 방법이 유용한 이유는 value값이 optional이 아닌 변수라는 것을 컴파일러가 인식하기 때문입니다.
아래와 같이 작성하면 컴파일러가 optional 변수라고 인식하여 컴파일 에러가 발생합니다.
{% highlight swift %}
if optionalValue != nil {
  value = optionalValue // value는 optional 변수가 아님
}
{% endhighlight %}

## 참고자료
[swift 문서 번역사이트](http://swift.leantra.kr/)
