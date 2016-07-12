---
layout: article
title: "digest 수정을 이용한 병목확인과 AngularJS 3차 성능개선(2)"
date: 2016-06-20 19:58:13
image:
  teaser: angularjs.png
categories: technology
---

이 글은 AngularJS의 웹 어플리케이션 성능개선을 진행한 방법을 정리한 글입니다.  

- OSX 크롬
- AngularJS v1.5.7  

## $rootScope.$digest 호출 숫자가 줄었는데도 성능 개선이 되지 않은 이유  

 저번 글인 [AngularJs 3차 성능개선](http://kbs0327.github.io/blog/technology/angularjs-improve-performance3/)에서는 AngularJS에서 주된 성능병목 중의 하나인 $rootScope.$digest가 호출되는 수를 줄여서 성능개선을 시도했었습니다.  

 하지만 위의 소제목에서도 보이다시피 실제 성능은 평균 페이지로딩 속도 기준으로 2.74초에서 3.07초로 시간이 늘어나서 오히려 성능이 안좋아졌습니다. 그 원인을 찾기 위해서 변경했던 소스들을 하나씩 지워보기도 하고 측정이 잘못되었는지도 반복해서 확인을 해보았습니다. 그래서 내린 결론은 처음의 가정과는 다르게 $rootScope.$digest를 적게 호출해서 느려졌다는 결론이었습니다.  

 그러면 $rootScope.$digest를 더 적게 호출했는데 왜 성능이 느려진 것일까요? 상식적으로 생각했을 때에 $rootScope.$digest는 컴퓨터의 여러자원을 불필요하게 사용하는 것이어서 성능이 저하되어야 합니다. 결론부터 말씀드리자면 `$httpProvider.useApplyAsync = true` 때문이었습니다. 처음 AngularJS 성능개선 글을 쓸 때부터 useApplyAsync를 true로 설정하면 성능이 좋아진다고 말씀드렸습니다. 하지만 지금은 이 옵션으로 인해서 성능개선이 안되었다고 말씀드리고 있습니다. 이 이유를 하나씩 알려드리겠습니다.  

### AngularJS 화면 로딩 및 스크립트 실행

 AngularJS는 $watch와 $digest를 통하여 화면을 로딩합니다. 먼저 HTML이 렌더링이 되고 HTML에 있는 directive의 스크립트들이 실행됩니다. directive의 스크립트들이 실행이 될 때에 HTML을 로딩하는 directive들도 실행이 되게 됩니다. 그리고 이 행위가 반복이 되면서 화면이 그려지게 됩니다.  

 이렇게 화면을 로딩할 때 두레이의 경우 SPA(Single page application)를 사용하므로 중간에 API를 AJAX를 통해서 호출하게 됩니다. 그리고 이 응답을 가지고 위의 행위를 다시 시작하게 되어 완성된 화면을 그리게 됩니다. AngularJS에서는 이를 도와주기 위하여 매번 AJAX 호출이 있은 후에 $rootScope.$digest를 호출하여 바로 다시 화면을 그리는 작업을 시작하게 합니다.  

### useApplyAsync 옵션 
 
 여기서 useApplyAsync 옵션이 하는 일을 다시 기술해보면 아래와 같습니다.

> 요약하면 `$http` 서비스에서는 HTTP request의 응답이 올 때에 `$digest`를 호출합니다. 이 속성을 켜면 동시에 많은 HTTP request를 보낼 때에 묶어서 `$digest`를 1번만 보내게 한다는 것입니다. 즉, 이 속성을 켜면 `$digest` 호출 수를 줄여서 성능을 개선할 수 있습니다.  

 위에서 나온 것처럼 useApplyAsync 옵션을 키면 AJAX 응답이 오자마자 발생하는 $rootScope.$digest를 일정시간동안 지연시킵니다. 즉, 화면을 더 늦게 그리게 되는 것입니다. 두레이에서는 또한 AJAX응답으로 화면을 그린 후에 다시 AJAX 호출을 하고 그 데이터로 화면을 그리는 작업을 반복합니다. 그렇기에 불필요한 $rootScope.$digest호출이 줄어들자 성능이 오히려 나빠지는 현상이 일어났던 것입니다.  

## useApplyAsync와 $applyAsync

 useApplyAsync를 true로 호출하면 AJAX호출을 한 후에 $apply함수가 아닌 $applyAsync함수를 사용하게 합니다. $applyAsync 함수를 더 자세히 알아보겠습니다.  

``` js
$applyAsync: function(expr) {
  var scope = this;
  expr && applyAsyncQueue.push($applyAsyncExpression);
  expr = $parse(expr);
  scheduleApplyAsync();

  function $applyAsyncExpression() {
    scope.$eval(expr);
  }
}
```

 위의 함수는 AngularJS에 있는 실제 $applyAsync의 코드입니다. 코드를 잠시 살펴보면 expr이 있으면 그 코드를 실행하는 콜백을 `applyAsyncQueue`에 넣어서 보관하고 `scheduleApplyAsync` 함수를 실행합니다.  

``` js
function flushApplyAsync() {
  while (applyAsyncQueue.length) {
    try {
      applyAsyncQueue.shift()();
    } catch (e) {
      $exceptionHandler(e);
    }
  }
  applyAsyncId = null;
}

function scheduleApplyAsync() {
  if (applyAsyncId === null) {
    applyAsyncId = $browser.defer(function() {
      $rootScope.$apply(flushApplyAsync);
    });
  }
}
```

 `scheduleApplyAsync` 함수는 위처럼 미리 대기중인 `applyAsyncId`가 있으면 아무 것도 안하지만 `applyAsyncId`가 없을 시에 `$browser.defer` 함수로 `flushApplyAsync`함수를 실행합니다. 여기서 `$browser.defer`함수는 timeout 함수로 보아도 무방합니다. timeout이 끝나면 `applyAsyncQueue`에 들어있는 값들을 하나씩 꺼내서 실행시킵니다. 물론 `$apply`함수를 호출하여 $rootScope.$digest 또한 호출됩니다.  

### $applyAsync에 대기하고 있는 콜백을 빨리 실행시키기

 그러면 $applyAsync를 통해 `applyAsyncQueue`에 들어있는 콜백들을 빠르게 호출하는 방법은 없을까요? 그 방법은 $rootScope.$digest를 호출하는 것입니다. $digest의 소스를 보면 아래와 같은 함수가 있습니다. 

``` js
if (this === $rootScope && applyAsyncId !== null) {
  $browser.defer.cancel(applyAsyncId);
  flushApplyAsync();
}
```
 
 즉, $rootScope.$digest를 호출했을 때에 `flushApplyAsync` 함수를 강제로 실행시는 것입니다. 이 방법을 이용하면 불필요했던 $rootScope.$digest가 아닌 제가 의도한 바에 따라 속도를 빠르게 할 수 있습니다.  

### 의도한 $rootScope.$digest를 사용하여 성능 향상시키기  

 먼저 아래와 같은 함수를 만들어서 현재 $digest가 호출 중이지 않을 때에만 $rootScope.$digest를 호출하게 했습니다.  

``` js
function () {
  if (!$rootScope.$$phase) {
    $rootScope.$apply();
  }
}
```

 이 함수를 적절한 위치(주로 API가 호출한 결과가 바로 화면에 나타나야하는 곳들)에서 실행시키고 결과를 측정해보았습니다.  

 먼저 $rootScope.$digest를 최대한 제거하고 측정한 속도는 3.07초였습니다. 1개씩 추가시킬 때 2.95초, 2.87초, 2.73초, 2.70초로 속도를 증가 시킬 수 있었습니다. 이 수치를 보면 속도개선이 많이 되었는지 체감이 되지 않습니다.  

| 초기값 | 1개 추가 | 2개 추가 | 3개 추가 | 4개 추가 |
| ---  | ---    | ---     | ---    | ---    |
| 3.07 | 2.95   | 2.87    | 2.73   | 2.70   |

성능측정 페이지(1)

 하지만 다른 페이지의 로딩 속도를 보면 차이가 확연히 드러납니다. 먼저 아무런 개선도 하지 않았을 때가 3.74초였습니다. 그리고 $rootScope.$digest를 제거시킬 수 있는만큼 제거하였을 때에 3.42초로 성능을 개선시켰습니다.(이 페이지의 경우에는 $rootScope.$digest만 제거했을 때에도 성능이 개선되었습니다.) 그리고 적절한 $rootScope.$digest를 호출시키자 3.25초까지 성능을 개선할 수 있었습니다.  

| 개선 전 | $rootScope.$digest 제거 | $rootScope.$digest 임의 추가 | 
| ---   | ---                    | ---                        |
| 3.74  | 3.42                   | 3.25                       |

성능측정 페이지(2)

### 같은 행동을 했는데 성능향상에 차이가 있었던 이유  

 위처럼 $rootScope.$digest만 제거했을 때에도 성능이 개선된 경우는 어떤 경우일까요? 이것은 watch 확인 수로 알 수 있었습니다.  
 전자의 $rootScope.$digest가 제거되었을 때 성능이 저하된 경우 $rootScope.$digest 제거 전에는 24774번의 watch를 확인했습니다. 그리고 이것이 $rootScope.$digest 제거 후에 6246번으로 줄었습니다.  
 하지만 후자의 $rootScope.$digest가 제거되었을 때에도 성능이 향상된 경우에는 $rootScope.$digest 제거 전에는 37833번의 watch를 확인했습니다. 그리고 $rootScope.$digest 제거 후에는 12273번으로 줄었습니다. 즉, watch를 확인하는데에 시간이 AJAX가 응답하는 시간보다 오래걸려서 위의 useApplyAsync 옵션으로 인한 성능저하가 되지 않았던 것입니다.  


| 성능측정 페이지  | $rootScope.$digest 제거 전 | $rootScope.$digest 제거 후 | 
| ---          | ---                      | ---                       |
| 1번 페이지     | 24774                    | 6246                      |
| 2번 페이지     | 37833                    | 12273                     |

$rootScope.$digest 제거 전과 후의 페이지별 watch 수

## 결론  

 이번 개선을 통해서 성능을 향상시키다 보면 한계를 만나게 되고 이 때가 되면 기존의 성능개선을 하려고 사용했던 부분이 성능개선의 병목으로 작용할 수 있다는 것을 알았습니다. 혹시 성능개선을 하면서 분명 성능이 개선되어야 하는데 성능 개선이 안된다고 한다면 기존의 성능개선이 병목으로 작용할 수는 없는지 알아보는 것이 좋을 것 같습니다.  
