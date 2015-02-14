---
layout: post
title:  "Java8 default 인터페이스"
date:   2015-02-15 01:13:13
image: 
  teaser: java8.png
categories: technology
---

# JAVA8에 추가된 기능 - default interface
  
 2014년 3월 18일에 JAVA8이 공식 배포가 되었습니다.  
 이 글에서는 JAVA8에서 추가된 기능 중에 default interface에 대해서 설명하겠습니다.  
  
## default interface 개요
  
 interface를 떠올려보면 아래와 같이 method의 구현은 없고 선언만 있는 모습을 떠올리기 쉽습니다.  
{% highlight java %}
public interface Interface {
	int aaa();
	int bbb();
}
{% endhighlight %}
하지만 JAVA8에서는 interface에 아래와 같이 method들을 정의하는 것이 가능해졌습니다.  
바로 default method와 static method입니다.
```java
public interface Interface {
	int aaa();
	int bbb();
	default int ccc() {
		return 0;
	}
	
	static int ddd() {
		return 0;
	}
}
```
이렇게 인터페이스에 기본적으로 적용이 되는 method들을 추가 시키는 것입니다.  
이 default method는 3가지 방법으로 사용이 됩니다.   
* 아무런 구현이 없이 인터페이스의 method를 사용하는 방법
```java
public class ImplementingClass implements Interface{
	@Override
	public int aaa() {
		int a = ccc();
		return a;
	}
	@Override
	public int bbb() {
		int b = Interface.ddd();
		return b;
	}
}
```
* default method가 선언된 interface를 상속받아서 다시 abstract method로 만드는 방법
```java
public interface InheritingingInterface extends Interface{
	int ccc();
}
```
* default method가 선언된 interface를 상속받아서 default method를 override하는 방법
```java
public interface InheritingingInterface extends Interface{
	default int ccc() {
		return 2;
	}
}
```  

**단, default method로 Object 클래스의 method를 override하는 것은 할 수 없습니다.**
  
## default interface가 생긴 이유 
  
default method는 interface가 그 동안 막아왔던 함수의 구현을 부분적으로 허용하게 하였습니다.  
[Java 8 explained: Default Methods](http://zeroturnaround.com/rebellabs/java-8-explained-default-methods/)에서는 그 이유에 대해 아래와 같이 설명하고 있습니다.
  
이미 JAVA는 오랜시간동안 사용되었습니다.  
그런데 람다라는 개념을 넣으면서 interface의 내용이 추가될 필요가 생겼습니다.  
여기서 interface에 method를 추가하려면 interface를 구현한 모든 클래스에도 해당 method를 구현해야 한다는 문제가 발생합니다.  
또한 기존의 버전과 호환성 문제도 있고 사용자가 JAVA 기본 interface를 구현한 경우 문제가 발생합니다.  
그렇기 때문에 interface에 default method와 static method를 추가했습니다.  
  
## default interface에 대한 의문점들  
  
### default method의 diamond 상속문제

자바는 다중상속이 되지 않습니다. 그래서 diamond상속문제가 그 동안은 없었습니다.  
하지만 interface에 method를 추가시킨다는 개념이 도입되면서 diamond상속문제에 대해서 어떻게 동작하는지 의문이 들었습니다. 
  
####실험1 
먼저 default method인 ccc를 상속받아서 override하는 2개의 interface를 만들었습니다.  
```java
public interface AInterface extends Interface {
	default int ccc() {
		return 1;
	}
}

public interface BInterface extends Interface{
	default int ccc() {
		return 2;
	}
}
```
그리고 그 둘을 구현하는 Diamond class를 만들었습니다.
```java
public class DiamondClass implements AInterface, BInterface {
	@Override
	public int aaa() {
		return 0;
	}
	
	@Override
	public int bbb() {
		return 0;
	}
}
```
그러자 이클립스에서 아래와 같은 에러를 보여줍니다.
```
Duplicate default methods named ccc with the parameters () and () are inherited from the types BInterface and AInterface
```
####실험2
다음으로 AInterface만 ccc method를 구현해 보겠습니다.
```java
public interface Interface {
	int aaa();
	int bbb();
	default int ccc() {
		return 0;
	}
	
	static int ddd() {
		return 0;
	}
}

public interface AInterface extends Interface {
	default int ccc() {
		return 1;
	}
}

public interface BInterface extends Interface{
}

public class DiamondClassTest {
	DiamondClass diamondClass = new DiamondClass();
	@Test
	public void testDiamond() throws Exception {
		assertThat(diamondClass.ccc(), is(1));
	}
}
```
결과는 실패로 나왔고 ccc() method의 return값은 0이었습니다.  
즉, AInterface의 override한 default method가 아니라 Interface의 default method를 호출한 것이었습니다.  
  
####실험3
이번에는 상속 받는 순서를 바꾸고 테스트해보겠습니다.
```java
public class DiamondClass implements BInterface, AInterface
```
그래도 똑같은 결과가 나옵니다.  

####실험4
이번에는 AInterface와 BInterface에 아래와 같이 변경하고 테스트 해보겠습니다.  
```java
public interface AInterface {
	default int ccc() {
		return 1;
	}
}

public interface BInterface {
	default int ccc() {
		return 2;
	}
}
```
이번에도 아래와 같은 메세지가 나옵니다.
```
Duplicate default methods named ccc with the parameters () and () are inherited from the types AInterface and BInterface
```
  
**결론**  
**1. 실험1과 실험4로 보아 구현하는 interface에 같은 method의 형태가 있을 때는 에러가 발생**  
**2. 실험2와 실험3으로 보아 diamond상속이 있으면 interface의 최상위에 정의한 default method가 호출됩니다.**  

### static method 상속 문제  
  
static method는 어떤 형태로 호출되는지에 대해서 확인해보겠습니다.
  
####실험1
static method를 구현한 클래스에서 그냥 호출해 보겠습니다.
```java
public interface Interface {
	int aaa();
	int bbb();
	default int ccc() {
		return 0;
	}
	
	static int ddd() {
		return 0;
	}
}

public class ImplementingClass implements Interface{
	@Override
	public int aaa() {
		int a = ccc();
		return a;
	}
	
	@Override
	public int bbb() {
		int b = ddd();
		return b;
	}
}
```
아래와 같이 에러메세지가 나옵니다.
```
The method ddd() is undefined for the type ImplementingClass
```
즉, static method는 구현하는 클래스에서 method명만으로는 호출이 안됩니다.  
  
####실험2
Interface를 상속받는 interface의 경우에는 static method가 호출이 되는지 확인해보겠습니다.
```java
public interface InheritingingInterface extends Interface{
	default int ccc() {
		ddd();
	}
}
```
이것도 아래와 같이 에러메세지가 나옵니다.
```
The method ddd() is undefined for the type InheritingingInterface
```
  
**결론**  
* **static method의 경우 상속이나 구현을 해도 method를 호출하기 위해서는 '해당 클래스.method' 형태로 호출해야 합니다.**  
  
### default method는 상속의 상속시에도 사용할 수 있을까?  

default method가 과연 상속의 상속을 받아도 사용할 수 있는지 알아보겠습니다.  
  
####실험1
default method가 있는 interface를 구현한 클래스를 상속받은 클래스에서 default method의 사용이 가능한지 실험해보겠습니다.  
```java
public interface Interface {
	int aaa();
	int bbb();
	default int ccc() {
		return 1;
	}
	
	static int ddd() {
		return 0;
	}
}

public class ImplementingClass implements Interface{
	@Override
	public int aaa() {
		int a = ccc();
		return a;
	}
	
	@Override
	public int bbb() {
		int b = Interface.ddd();
		return b;
	}
}

public class InheritingImplementingClass extends ImplementingClass{
}

public class InheritingImplementingClassTest {
	InheritingImplementingClass inheritingImplementingClass = new InheritingImplementingClass();
	
	@Test
	public void testCallDefaultMethod() throws Exception {
		assertEquals(inheritingImplementingClass.ccc(), 1);
	}
}
```
이 테스트는 성공합니다.

####실험2  
이번에는 default method를 구현한 interface를 상속받은 interface를 구현한 클래스에서도 default method 호출이 가능한지 확인해 보겠습니다.
```java
public interface InheritingingInterface extends Interface{
}

public class ImplementingInheritingingInterface implements InheritingingInterface{
	@Override
	public int aaa() {
		return 0;
	}
	
	@Override
	public int bbb() {
		return 0;
	}
}

public class ImplementingInheritingingInterfaceTest {
	ImplementingInheritingingInterface implementingInheritingingInterface = new ImplementingInheritingingInterface();
	
	@Test
	public void testCallDefaultMethod() throws Exception {
		assertEquals(implementingInheritingingInterface.ccc(), 1);
	}
}
```
이 테스트도 성공합니다.  
  
**결론**  
* **실험1과 실험2 결과 default method를 구현한 interface의 자식들은 모두 이 default method를 사용할 수 있다는 것을 알 수 있습니다.**  

**이미지 출처**  
[http://techidiocy.com/metaspace-java8/](http://techidiocy.com/metaspace-java8/)  
**참고사이트**  
공식홈페이지 [http://docs.oracle.com/javase/tutorial/java/IandI/defaultmethods.html](http://docs.oracle.com/javase/tutorial/java/IandI/defaultmethods.html)  
Java 8 explained: Default Methods [http://zeroturnaround.com/rebellabs/java-8-explained-default-methods/](http://zeroturnaround.com/rebellabs/java-8-explained-default-methods/)  
Java 8 출시일 [https://www.java.com/ko/download/faq/release_dates.xml](https://www.java.com/ko/download/faq/release_dates.xml)  
Java 8 Interface Changes [http://www.journaldev.com/2752/java-8-interface-changes-static-methods-default-methods-functional-interfaces](http://www.journaldev.com/2752/java-8-interface-changes-static-methods-default-methods-functional-interfaces)  
