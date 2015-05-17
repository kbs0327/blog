---
layout: article
title: "Swift에서 Generic을 이용한 Constructor"
date: 2015-05-10 14:07:13
image:
  teaser: swift.png
categories: technology
---

# 개요
저희가 라이브러리에서 타입만 변화하는 중복코드들을 제거하기 위해 Generic을 많이 사용합니다.
그리고 이 글은 제가 ios 앱을 개발하던 중에 Generic의 이상동작이 있어서 쓴 글입니다.

## generic을 이용한 코드
Swift에서 Generic을 이용해서 만들어지는 코드는 크게 2가지 종류가 있습니다.

하나는 아래와 같은 함수의 매개변수로 타입을 지정하는 형태입니다.
{% highlight swift %}
func repeat<ItemType>(item: ItemType, times: Int) -> ItemType[] {
    var result = ItemType[]()
    for i in 0..times {
        result += item
    }
    return result
}
{% endhighlight %}

다른 하나는 class나 구조체를 정의할 때에 매개변수 타입을 지정하는 형태입니다.
{% highlight swift %}
enum OptionalValue<T> {
    case None
    case Some(T)
}
{% endhighlight %}

## Generic 제한
그리고 Generic을 선언할 때에 어떤 함수나 프로토타입의 메소드를 사용할 때에 Generic타입을 제한하여 메소드를 사용하게 할 수도 있습니다.
{% highlight swift %}
func anyCommonElements <T, U where T: Sequence, U: Sequence, T.GeneratorType.Element: Equatable, T.GeneratorType.Element == U.GeneratorType.Element> (lhs: T, rhs: U) -> Bool {
    for lhsItem in lhs {
        for rhsItem in rhs {
            if lhsItem == rhsItem {
                return true
            }
        }
    }
    return false
}
anyCommonElements([1, 2, 3], [3])
{% endhighlight %}

## Generic Constructor
여기서 Generic을 이용한 Constructor를 만들면 어떻게 될지 한번 실험해 보았습니다.
{% highlight swift %}
class GenericClass<T> {
    func function() {
        return T()
    }
}
{% endhighlight %}
이렇게 선언할 시에 initializer가 없다고 하면서 에러가 납니다.

하지만 여기서 Generic을 제한하면 어떻게 될지 알아보겠습니다.
{% highlight swift %}
class SuperModel {
    func function() {
        println("SuperModel 출력")
    }
}

class GenericClass<T where T : SuperModel> {
    func executeFunction() -> T {
        return T()
    }
}
GenericClass<SuperModel>().executeFunction().function()
{% endhighlight %}
이렇게 선언하면 'SuperModel 출력'이 출력됩니다.

## Generic을 이용한 다양한 Constructor
그러면 Generic을 제한하고 SubClass들을 생성하게 한다면 어떻게 될지 확인해 보겠습니다.
{% highlight swift %}
class SuperModel {
    func function() {
        println("SuperModel 출력")
    }
}

class SubModel : SuperModel {
    override func function() {
        println("SubModel 출력")
    }
}

class GenericClass<T where T : SuperModel> {
    func executeFunction() -> T {
        return T()
    }
}
GenericClass<SubModel>().executeFunction().function()
{% endhighlight %}
이렇게 코드를 실행시키면 어떤 값이 나올까요?
제가 예상한 값은 'SubModel 출력'이라는 값입니다.

하지만 위의 코드를 실행시키면 'SuperModel 출력'이라는 결과가 나옵니다.
즉, Swift에서는 Generic을 이용하여 다양한 SubClass를 생성해서 사용하는 것은 할 수 없습니다.

## 정리
- Swift에서는 Generic을 함수와 클래스, 2가지 형태로 선언, 정의할 수 있습니다.
- Generic을 이용한 생성자는 Generic을 제한할 때에만 사용이 가능합니다.
- 하지만 Generic을 이용하여 SubClass들을 생성하려고 하는 것은 불가능합니다.

즉, 객체를 생성할 때에 Generic을 사용할 수는 없습니다.

## 참고자료
[swift 문서 번역사이트](http://swift.leantra.kr/)
