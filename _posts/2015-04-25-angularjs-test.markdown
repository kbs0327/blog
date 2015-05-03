---
layout: article
title: "AngularJs Test"
date: 2015-04-25 14:07:13
image:
  teaser: angularjs.png
categories: technology
---

# AngularJs Test
AngularJs로 작성한 코드를 테스트하는 방법을 정리했습니다.

## AngularJs에 대한 이해
AngularJs는 자바스크립트를 구동하는 프레임워크이므로 암묵적으로 작동하는 코드들이 있습니다.  
그렇기 때문에 AngularJs에 대한 간단한 이해가 있어야 테스트 코드를 작성할 수 있습니다.

### 구성
AngularJs는 기본 설정을 하는 Bootstrap과정이 있고, 그 과정에서 Router설정을 할 수 있습니다.  
여기서 Bootstrap이란 프로그램의 실행 환경을 설정하는 것을 말하고  
Router설정은 각 URL별로 어떤 뷰를 보여줄지 정하는 방식입니다.  
그리고 Bootstrap과정에서 컨트롤러, 서비스, 팩토리, 지시자 등을 조합해서 전체 프로그램이 구성됩니다.  
이와 관련된 내용은 [AngularJs 공식 홈페이지 Developer Guide](https://docs.angularjs.org/guide)를 참고하면 됩니다.  

## Jasmine & Karma Test
AngularJs는 기본적으로 자스민으로 테스트 코드를 작성합니다.(공식 홈페이지 기준)   

## 테스트 어려운 형태 테스트
AngularJs에서 테스트를 할 때 기본적인 사항은 기본 자스민 테스트방식과 유사하며,  
이는 [자스민 공식홈페이지](http://jasmine.github.io/)나 [블로그 글](http://kbs0327.github.io/blog/technology/jasmine/)을 참고하시면 됩니다.  

### 테스트 케이스 기본설정
AngularJs로 테스트케이스를 작성할 때 가장 난감한 부분이 테스트 환경을 구축하는 부분입니다.  
이는 AngularJs에서 app에 컨트롤러, 서비스, 팩토리, 지시자 등이 종속되어 있기 때문에 더 그렇습니다.  
그렇기 때문에 아래처럼 테스트 코드를 작성하기 전에 app을 먼저 읽어와서 설정을 해줘야합니다.  

{% highlight js %}
beforeEach(module('앱이름'));
{% endhighlight %}

### 컨트롤러 테스트 케이스 설정
다른 테스트 케이스들은 앱설정만 하면 어느정도의 테스트 케이스는 만들 수 있습니다.  
하지만 컨트롤러의 테스트 케이스는 조금 다른 점이 있는데,  
이 점은 컨트롤러가 DOM과의 연관성이 가장 높기 때문입니다.  
그래서 컨트롤러의 테스트 케이스 설정방법은 조금 다릅니다.  

{% highlight js %}
beforeEach(inject(function ($controller, $rootScope) {	// injector를 통해 $controller와 $rootScope를 inject받음
        scope = $rootScope.$new();			// $rootScope.$new()를 위해 컨트롤러가 동작할 scope를 설정합니다.

        ContainerCtrl = $controller('MyCtrl', {		// MyCtrl의 컨트롤러를 $element와 $scope에 아래의 값을 넣어서 만듭니다.
            $element : angular.element('<div/>'),	// <div/>만 들어가 있는 DOM을 $element에 할당합니다.
            $scope: scope				// $scope에 목으로 만든 scope객체를 할당합니다.
        });
}
{% endhighlight %}
위와 같은 형식으로 컨트롤러 테스트 케이스를 작성하는데,  
이렇게 작성한 테스트 환경을 작동하면 실제 컨트롤러의 코드에서 scope를 접근하는 것을 mocking할 수 있고,
또한 컨트롤러가 선언된 DOM 트리를 지정해 줄 수 있습니다.  
그리고 컨트롤러에서 $element를 사용한다면, 꼭 mocking을 해줘야 합니다.  

### $httpBackend
테스트를 하면 서버에 어떤 요청을 했는지 확인하고, 요청에 대한 응답을 정하고 싶을 때가 있습니다.
그 때에 사용하는 것이 $httpBackend입니다. 아래 내용은 공식홈페이지에 나와있는 $httpBackend에 관련된 내용입니다.  

$http 서비스를 테스트할 때 사용합니다.  
테스트하는 동안 서버에 XHR이나 JSONP가 요청되지 않기를 원합니다.  
http를 테스트 하는데는 2가지 종류로 테스트가 가능합니다.  

$httpBackend.expect: 정확한 request가 예상될때(해당 요청이 안와도 에러가 나옴)  
$httpBackend.when: backend를 정의할 때(어떤 요청에 대해 어떤 답을 주라는 설정)  
flush() 메소드를 실행하면 해당요청에 대한 설정한 반환값이 반환됩니다.  

그리고 아래는 $httpBackend를 사용하는 예입니다.
{% highlight js %}
// $httpBackend mock 설정 부분
$httpBackend.expectGET('test할 URL')
            .respond({
                기대하는 응답값
            });

// 테스트 코드..

$httpBackend.flush(); // http요청에 대한 호출
{% endhighlight %}

### $timeout
$timeout은 몇 ms 후에 코드를 실행시키게 하는 코드인데, 테스트를 할 때는 그 시간을 기다릴 필요가 없습니다.  
그래서 $timeout을 테스트 하기 위해서는 $timeout의 안의 코드를 강제로 실행시켜야 합니다.  
그 때에 아래처럼 실행 시킬 수 있습니다.

{% highlight js %}
// 테스트 코드..

$timeout.flush();
{% endhighlight %}

### $provide
테스트를 할 때에 controller에 들어가는 library 객체들을 수정하고 싶을 때가 있습니다.  
그럴 때 사용하는 것이 $provider입니다.  
$provide를 통해 해당 코드가 동작하는 환경을 변경할 수 있습니다.  
사용방법은 아래와 같습니다.  

{% highlight js %}
beforeEach(mock(function($provide) {
	$provider.value(...);
}));
{% endhighlight %}

### $watch
$watch는 AngularJs에서 어떤 객체의 변화를 계속 확인하다가 그 값이 변경되면 다른 함수를 호출하는 것입니다.  
$watch를 테스트 할 때는 scope에 $digest를 호출하는데 그렇게 되면 $watch로 확인하는 값이 변경되었는지 한번 더 확인합니다.  
그러니 $watch로 감시하는 값을 변경시키고 $digest를 호출하면 해당 함수를 테스트할 수 있습니다.  
$watch와 관련된 내용은 [이 블로그](http://mobicon.tistory.com/328)에서 자세히 나옵니다.  

### $on
$on은 AngularJs에서 이벤트의 발생을 기다리다가 이벤트가 발생하면 다른 함수를 호출하는 역할을 합니다.  
$on을 테스트 할 때는 $broadcast와 $emit을 통해서 이벤트를 강제로 발생시켜서 해당 함수를 테스트 할 수 있습니다.  

## 참고자료  
[AngularJs 공식 홈페이지](https://angularjs.org/)  
[윤영식님 블로그](http://mobicon.tistory.com/328)  