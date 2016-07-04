---
layout: article
title: "digest 수정을 이용한 병목확인과 AngularJS 3차 성능개선"
date: 2016-06-20 19:58:13
image:
  teaser: angularjs.png
categories: technology
---

# 개요   

이 글은 AngularJS의 웹 어플리케이션 성능개선을 진행한 방법을 정리한 글입니다. 이번 글은 $rootScope.$digest가 어떤 곳에서 동작하는지 확인하고 그 수를 줄여서 성능개선을 시도한 내용입니다. 또한 이 글은 아래 환경을 기반으로 확인한 내용을 작성했습니다.  
- OSX 크롬
- AngularJS v1.5.7  

## AngularJS의 주요기능 watch와 digest  

 AngularJS의 가장 장점 중에 하나는 따로 이벤트리스너를 추가하지 않아도 ng-click, ng-keydown 등을 통하여 쉽게 이벤트를 핸들링할 수 있고, ng-bind, ng-bind-html 등을 통하여 데이터 변경을 쉽게 뷰에 반영시킬 수 있는 것입니다. 즉, 양방향바인딩이 된다는 것입니다. 이러한 것들을 구현하려면 어떻게 하면 될까요? 사실 답은 간단합니다. 어딘가에서 변경여부를 확인하고, 해당 변경에 대한 핸들러를 실행시켜주는 것입니다. 답은 간단하지만 구현을 할 때에는 고려할 사항들이 많이 있습니다. 얼마나 자주 변경을 확인할지, 한번 확인할 때 얼마나 많은 변경사항들을 확인할지 등에 대한 것을 결정하는 것입니다. 이러한 고려사항들을 효과적으로 구현한 것이 AngularJS라고 볼 수 있습니다.  
 AngularJs에서는 ng-bind, ng-bind-html 등의 데이터변경을 확인하여 어떤 행위를 하기 위해 [1차 성능개선](http://kbs0327.github.io/blog/technology/angularjs-improve-performance/)에서 간략하게 소개하고 있는 watch를 사용하고 있고, ng-click이나 ng-keydown등의 이벤트가 일어났을 때 digest를 작동시켜서 값을 확인하게 합니다.  
 조금 더 자세히 설명하자면 AngularJS에서 화면에 값을 보여주거나 화면의 이벤트를 기다리는 핸들러들은 대부분 scope의 변수나 메소드로 지정하게 됩니다. 화면에 값을 보여주는 ng-bind의 경우 scope의 특정 변수를 관찰하고 있다가 해당 변수가 변경되면 화면에 값을 변경해 주는데요 여기서 `관찰`하는 것이 watch이고 관찰하고 있는 값이 변경되었는지 확인하도록 알려주는 것이 digest입니다.  
 $rootScope.$digest를 호출하면 화면의 자식 scope들을 순회하면서 scope에 binding되어 있는 watch들에게 scope의 값이 변경되었는지 확인하게 하고, 이 값들은 ng-bind나 ng-bind-html같은 directive를 통해 화면에 나타나게 됩니다. 즉, scope는 rootScope를 root로 하는 트리구조로 되어 있어 $rootScope.$digest를 호출하면 모든 scope의 digest함수가 실행되게 됩니다. AngularJS 내부에서 대부분 digest를 호출할 때에는 $rootScope.$digest를 호출합니다. 이 이유는 자세히는 모르겠지만 부분적인 digest를 많이 호출하면 이로 인해 문제가 생겼을 때 추적이 어려워서 그런 것이 아닌지 추측하고 있습니다.  

 $rootScope.$digest를 호출시키는 것들을 알아보면 아래와 같습니다.  
 - ng-click, ng-keydown 디렉티브에서 해당 이벤트가 트리거될 때  
 - AJAX통신이 이루어질 때  
 - $rootScope.$digest, $apply, $applyAsync, $evalAsync 등이 호출될 때  

 위의 리스트를 보면 거의 모든 이벤트에 대해서 $rootScope.$digest를 호출한다는 것을 알 수 있습니다. 이렇게 자주 호출되기 때문에 사용자는 데이터의 변경이 바로 반영되는 것으로 보입니다. 하지만 이 방법은 문제가 있는데 웹어플리케이션이 커지면, watch수는 증가하고, $rootScope.$digest를 호출하는 이벤트들도 많이 호출되어 성능에 병목을 일으킵니다.  

 이러한 병목을 줄이려면 간단하게 2가지를 생각할 수 있습니다.  

 1. watch자체의 속도를 증가시킨다.(값이 변경되었는지 확인하는 로직을 가볍게 짭니다.)  
 2. $rootScope.$digest를 호출하는 이벤트들의 수를 줄입니다.  

 제가 1차 성능개선에서 one-time-binding을 하게 한다던지 $watch의 3번째 옵션에 true를 제거하는 등의 행위는 1번의 행위를 한 것입니다. (2차 성능개선은 watch와 관련되어 있다고 보기 어렵습니다.)  

 그래서 이번에는 2번인 $rootScope.$digest의 호출 수 를 줄여서 성능을 개선해보았습니다.  

### AngularJS에서 제공하는 $rootScope.$digest를 줄이는 방법  
 
 AngularJS에서도 이러한 사항을 알기에 이 문제를 해결할 수 있는 방법들이 있습니다.  

 먼저 $applyAsync가 있습니다. $applyAsync는 [1차 성능개선](http://kbs0327.github.io/blog/technology/angularjs-improve-performance/)에서 나온 useApplyAsync와 관련이 있는데요. AJAX호출을 하면 $apply함수를 호출합니다.($apply함수는 지금 $rootScope.$digest의 wrapper 메소드라고 보시면 됩니다.) 이 때에 useApplyAsync를 true로 setting하면 $applyAsync를 사용하게 되는데요. 이 함수는 지금 $rootScope.$digest가 실행중인지 확인하고, 실행중이라면 queue에 넣어서 실행중이지 않는 시점에 $rootScope.$digest를 한번만 실행하게 하는 함수입니다. 이 함수를 실행하면 동시에 여러번 AJAX콜을 할 때에 $rootScope.$digest를 줄여서 성능개선을 할 수 있습니다.  

 이 외에 $applyAsync와 유사한 함수로 $evalAsync함수가 있습니다. 이 함수에 대해서는 나중에 다시 글로 써서 올리겠습니다.  

### 가능하면 $rootScope.$digest가 호출이 안하는 옵션을 사용하자  
 
 AngularJS에서는 이벤트가 발생했을 때 $rootScope.$digest를 호출하지 않게 하는 옵션을 가지고 있는 함수가 있습니다. $timeout과 $interval인데요. 어떤 옵션도 주지 않았을 경우 $timeout은 timeout이 일어난 후에 $rootScope.$digest를 호출하고 $interval은 정해진 시간마다 내부 콜백을 실행하면서, $rootScope.$digest를 호출합니다.  

 이 2개의 함수 모두 3번째 매개변수에 false를 입력하면 $rootScope.$digest를 막을 수 있습니다. 하지만 실제로 $rootScope.$digest가 필요한 경우가 있기 때문에 사용할 때에 유의해야합니다.  

### local digest  

 AngularJS에서는 거의 모든 $digest가 $rootScope.$digest라고 보시면 됩니다. 하지만 localDigest를 유저가 구현할 수도 있는데요.  

```js  
 if ($rootScope.$$phase) {
 	$scope.$digest();
 }

```  

 간단하게 위의 소스를 이용하면 되지만 사용시에 여러 side effect가 있을 수도 있어서, 몇개의 블로그에서는 이를 Anti-Pattern으로 기술하기도 했습니다.  

## AngularJS 소스 수정을 통한 watch수 파악  

 드디어 본론으로 돌아와서 AngularJS소스를 수정해서 간단하게 수정해서 실제로 watch가 얼마나 오래걸리는지에 대해 측정해 보겠습니다.  
 이번 측정의 목표는 $rootScope.$digest를 어디서 호출하고, 얼마나 많이 호출하는지 확인하는 것과, 1개의 watch를 확인하는 데에 얼마나 많은 시간이 걸리는지 측정하는 것입니다.  

 제가 수정하기로 한 소스는 $watch를 확인하도록 하는 $digest함수입니다.  
 digest함수는 간단하게 표현하면 아래와 같습니다.  

```js  
 $digest: function() {
        // phase 시작
        
        do { 
        // 자식 scope를 순회
          do { 
          // scope의 전체 watch를 순회하며 변경사항 확인
          
          // 변경사항이 있으면 해당 action 실행 
          } while (...);

        } while (...);

        // phase 종료
      }
```  

위의 코드에서 저희가 측정하고 싶은 것은 $watch를 확인하는데 걸리는 시간이 얼마나 걸리는지를 측정해야 하니 위의 `scope의 전체 watch를 순회하며 변경사항 확인`하는 부분에 아래의 코드를 추가합니다.  

``` js  
 window.watchCount = window.watchCount + 1 || 1;
 var started = window.performance.now();
 // watch하는 변수가 변경되었는지 확인하는 코드
 var ellapsed = window.performance.now() - started;
 if (ellapsed > 100) {
     console.trace();
     console.error(window.watchCount + ': watch', watch.get, ellapsed);
 } else if (ellapsed > 10) {
     console.trace();
     console.warn(window.watchCount + ': watch', watch.get, ellapsed);
 } else if (ellapsed > 1) {
     console.log(window.watchCount + ': watch', watch.get, ellapsed);
 } 
```  
그러면 실제로 watch를 확인하는 횟수와 1번의 watch가 얼마나 시간이 오래 걸렸는지 측정됩니다. 또한 callstack을 출력하여 시간이 오래걸리는 watch의 경우 추적하여 개선할 수 있습니다.     

그리고 저희가 알고 싶었던 $rootScope.$digest가 얼마나 많이 호출되고, 몇번이나 호출되었는지 알기 위해서 `phase 시작` 부분에 아래와 같은 코드를 추가합니다.   
``` js   
 if (this === $rootScope) {
      window.rootScopeDigestCount = window.rootScopeDigestCount + 1 || 1;
      console.trace('rootScope digest: ' + window.rootScopeDigestCount);
  }
```    
그러면 $rootScope.$digest가 몇번을 돌았는지 어디서 rootScope.digest를 호출했는지 확인할 수 있습니다.  
정확한 측정을 위해서는 app.run 부분에 timeout을 이용하여 일정 시간 이후의 window.watchCount와 window.rootScopeDigestCount를 호출해보면 페이지를 로딩하는데 호출된 수를 객관적인 지표로 확인할 수 있습니다.  

위의 코드를 두레이 첫 화면인 담당업무함에 적용시켜보았습니다.  

### 결과  

먼저 위의 코드에 대한 결과입니다.  
$rootScope.$digest 수: 23번  
전체 watch 수 : 24774번  

그리고 $rootScope.$digest를 역으로 추적해서 $rootScope.$digest를 호출하는 요인을 최대한 제거했더니 아래와 같은 결과가 나왔습니다.    
$rootScope.$digest 수: 4번  
전체 watch 수 : 6246번  

이렇게 $rootScope.$digest 호출 수를 보면 성공적으로 성능개선을 했다는 것을 알 수 있습니다. 하지만 이렇게 성능을 개선하면 페이지 로딩 속도가 빨라질 것이라고 생각했습니다. 하지만 실제로 페이지 로딩속도를 측정해 보니 속도는 오히려 느려졌습니다. 이렇게 성능이 느려져서 성능개선은 시간 관계상 마무리할 수밖에 없었습니다. 하지만 이 다음에 진행했던 성능개선에서 느려진 것에 대한 원인을 파악하고 페이지 로딩 속도를 개선할 수 있었는데 이 내용은 다음 글에 이어서 적겠습니다.  
