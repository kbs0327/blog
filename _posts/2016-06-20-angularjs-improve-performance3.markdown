---
layout: article
title: "Angular.js 소스 수정을 이용한 AngularJS 3차 성능개선"
date: 2016-06-20 19:58:13
image:
  teaser: angularjs.png
categories: technology
---

# 개요  

이 글은 AngularJS의 웹 어플리케이션 성능개선을 진행한 방법을 정리한 글입니다. 맥 크롬 기반으로 글을 작성했습니다.  
- AngularJS v1.5.7 기준으로 작성되었습니다.  

 [AngularJS 2차 성능개선](http://kbs0327.github.io/blog/technology/angularjs-improve-performance2/)에서 언급한 $rootScope.$digest 수를 줄여서 성능개선을 시도한 내용을 중심으로 말씀드리겠습니다.  

## AngularJS의 주요기능 Watch  

 angularJS의 가장 장점 중에 하나는 따로 이벤트 바인딩 등을 하지 않아도 ng-click, ng-keydown 등을 통하여 쉽게 이벤트를 핸들링할 수 있고, ng-bind, ng-bind-html 등을 통하여 데이터 변경을 쉽게 뷰에 반영시킬 수 있는 것입니다. 이러한 것들이 가능하려면 어딘가에서 변경여부를 확인하고, 해당 변경에 대한 핸들러를 실행시켜야할 것입니다. 즉, 유저가 하는 일을 프레임워크인 AngularJS가 대신해주는 것입니다.  
 이러한 ng-click, ng-keydown 이벤트나 데이터 변경을 확인하는 것은 [1차 성능개선](http://kbs0327.github.io/blog/technology/angularjs-improve-performance/)에서 간략하게 소개하고 있는 watch를 통해서 이루어집니다. AngularJS는 그러면 watch를 어떤 방식으로 값을 확인할까요? AngularJS는 $scope으로 이루어져 있고 $scope의 최상위에는 $rootScope가 있습니다. 이 $rootScope에 $digest함수가 실행되면, $rootScope의 $watch들이 값이 변했는지 확인해보고 그 자식을 확인, 자식의 자식을 확인 하는 방식으로 모든 watch의 변경사항을 확인합니다. 그래서 어떤 이벤트가 트리거되면 $rootScope.$digest()를 호출합니다.  

 $rootScope.$digest를 호출시키는 것들  
 - ng-click, ng-keydown 디렉티브에서 해당 이벤트가 트리거될 때  
 - AJAX통신이 이루어질 때  
 - $rootScope.$digest, $apply, $applyAsync, $evalAsync 등이 호출될 때  

 위의 리스트를 보면 거의 모든 이벤트에 대해서 $rootScope.$digest를 호출한다는 것을 알 수 있습니다. 이렇게 자주 호출되기 때문에 사용자는 데이터가 바로바로 반영되는 것으로 보입니다. 하지만 이 방법은 문제가 있을 수밖에 없는데 웹어플리케이션이 커지면, watch수는 증가하고, $rootScope.$digest를 호출하는 이벤트들도 많이 호출되어 성능에 병목을 일으킵니다.  

 이러한 병목을 줄이려면 간단하게 2가지를 생각할 수 있습니다.  

 1. watch자체의 속도를 증가시킨다.(값이 변경되었는지 확인하는 로직을 가볍게 짭니다.)  
 2. $rootScope.$digest를 호출하는 이벤트들의 수를 줄입니다.  

 제가 1차 성능개선에서 one-time-binding을 하게 한다던지 $watch의 3번째 옵션에 true를 제거하는 등의 행위는 1번의 행위를 한 것입니다. (2차 성능개선에서 한 것은 1번에도 해당하지만 병목을 lazy loading한 것에 가까워 제외했습니다.)  

 그래서 이번에는 2번인 $rootScope.$digest의 호출 수 를 줄여서 성능을 개선해보겠습니다.  

 ### AngularJS에서 제공하는 $rootScope.$digest를 줄이는 방법  
 
 AngularJS에서도 이러한 사항을 알기에 이 문제를 해결할 수 있는 방법들이 있습니다.  

 먼저 $applyAsync가 있습니다. $applyAsync는 [1차 성능개선](http://kbs0327.github.io/blog/technology/angularjs-improve-performance/)에서 나온 useApplyAsync와 관련이 있는데요. AJAX호출을 하면 $apply함수를 호출합니다.($apply함수는 지금 $rootScope.$digest의 wrapper 메소드라고 보시면 됩니다.) 이 때에 useApplyAsync가 true로 setting하면 $applyAsync를 사용하게 되는데요. 이 함수는 지금 $rootScope.$digest가 실행중인지 확인하고, 실행중이라면 queue에 넣어서 실행중이지 않는 시점에 $rootScope.$digest를 한번만 실행하게 하는 함수입니다. 이 함수를 실행하면 동시에 여러번 AJAX콜을 할 때에 $rootScope.$digest를 줄일 수 있습니다.  

 이 외에 이 함수와 똑같은 기능을 하는 함수로 $evalAsync함수가 있습니다. 이 함수와 $applyAsync함수의 차이점은 내부로직이 약간 다를뿐 같다고 생각해도 됩니다. 이 2가지 함수를 사용하면 $rootScope.$digest의 수를 줄이기 쉽습니다.  

 ### $rootScope.$digest가 호출이 안되게 하는 옵션을 사용하자
 
 AngularJS에서는 이벤트가 발생했을 때 $rootScope.$digest를 호출하지 않게 하는 함수가 있습니다. $timeout과 $interval인데요. $timeout은 timeout이 일어난 후에 $rootScope.$digest를 호출하고 $interval은 정해진 시간마다 내부 콜백을 실행하고 $rootScope.$digest를 호출합니다.  

 이 2개의 함수 모두 3번째 매개변수에 false를 입력하면 $rootScope.$digest를 막을 수 있습니다. 하지만 실제로 $rootScope.$digest가 필요한 경우가 있기 때문에 사용할 때에 유의하여 사용해야합니다.  

 ### local digest  

 AngularJS에서는 거의 모든 $digest가 $rootScope.$digest라고 보시면 됩니다. 하지만 localDigest를 유저가 구현할 수도 있는데요.

 ```js  
 if ($rootScope.$$phase) {
 	$scope.$digest();
 }

 ```  

 위의 소스를 이용하면 되지만 사용시에 추적이 힘드는 side effect가 있을 수도 있습니다. 실제로 몇개의 블로그에서는 이를 Anti-Pattern으로 기술하기도 했습니다.  

 ## AngularJS 소스를 통한 watch수 파악  

 드디어 본론으로 돌아와서 AngularJS소스를 수정해서 성능을 개선해보겠습니다.  
 이번 성능개선의 목표는 $rootScope.$digest 호출을 줄여서 처음 로딩 속도를 증가시키는게 주요 목적입니다.  

 제가 수정하기로 한 소스는 $digest함수 소스이고 이 소스가 모든 $watch의 확인에 사용되기에 이 함수를 target으로 지정했습니다.  

 ```js
 $digest: function() {
        beginPhase('$digest');
        $browser.$$checkUrlChange();

        if (this === $rootScope && applyAsyncId !== null) {
          $browser.defer.cancel(applyAsyncId);
          flushApplyAsync();
        }

        lastDirtyWatch = null;

        do { // "while dirty" loop
          dirty = false;
          current = target;

          for (var asyncQueuePosition = 0; asyncQueuePosition < asyncQueue.length; asyncQueuePosition++) {
            try {
              asyncTask = asyncQueue[asyncQueuePosition];
              asyncTask.scope.$eval(asyncTask.expression, asyncTask.locals);
            } catch (e) {
              $exceptionHandler(e);
            }
            lastDirtyWatch = null;
          }
          asyncQueue.length = 0;

          traverseScopesLoop:
          do { // "traverse the scopes" loop
            if ((watchers = current.$$watchers)) {
              length = watchers.length;
              while (length--) {
                try {
                  watch = watchers[length];
                  if (watch) {
                    get = watch.get;

                    // ---------------- watch 확인 ----------------
                    if ((value = get(current)) !== (last = watch.last) &&
                        !(watch.eq
                            ? equals(value, last)
                            : (typeof value === 'number' && typeof last === 'number'
                               && isNaN(value) && isNaN(last)))) {
                      // ---------------  watch 함수 실행 -------------
                      dirty = true;
                      lastDirtyWatch = watch;
                      watch.last = watch.eq ? copy(value, null) : value;
                      fn = watch.fn;
                      fn(value, ((last === initWatchVal) ? value : last), current);
                      if (ttl < 5) {
                        logIdx = 4 - ttl;
                        if (!watchLog[logIdx]) watchLog[logIdx] = [];
                        watchLog[logIdx].push({
                          msg: isFunction(watch.exp) ? 'fn: ' + (watch.exp.name || watch.exp.toString()) : watch.exp,
                          newVal: value,
                          oldVal: last
                        });
                      }
                    } else if (watch === lastDirtyWatch) {
                      dirty = false;
                      break traverseScopesLoop;
                    }
                  }
                } catch (e) {
                  $exceptionHandler(e);
                }
              }
            }

            if (!(next = ((current.$$watchersCount && current.$$childHead) ||
                (current !== target && current.$$nextSibling)))) {
              while (current !== target && !(next = current.$$nextSibling)) {
                current = current.$parent;
              }
            }
          } while ((current = next));

          if ((dirty || asyncQueue.length) && !(ttl--)) {
            clearPhase();
            throw $rootScopeMinErr('infdig',
                '{0} $digest() iterations reached. Aborting!\n' +
                'Watchers fired in the last 5 iterations: {1}',
                TTL, watchLog);
          }

        } while (dirty || asyncQueue.length);

        clearPhase();
        while (postDigestQueuePosition < postDigestQueue.length) {
          try {
            postDigestQueue[postDigestQueuePosition++]();
          } catch (e) {
            $exceptionHandler(e);
          }
        }
        postDigestQueue.length = postDigestQueuePosition = 0;
      }
 ```

코드가 정말 장황하지만 가장 중요한 지점은 watch 확인과 watch 실행하는 부분입니다.  
이 watch를 확인하는 부분에 아래의 코드를 집어넣습니다.

``` js
 window.watchCount = window.watchCount + 1 || 1;
var started = window.performance.now();
(value = get(current)) !== (last = watch.last) &&
    !(watch.eq
        ? equals(value, last)
        : (typeof value === 'number' && typeof last === 'number'
           && isNaN(value) && isNaN(last)))
var ellapsed = window.performance.now() - started;
if (ellapsed > 100) {
  console.trace();
  console.error(window.watchCount + ': watch', watch.get, ellapsed);
} else if (ellapsed > 10) {
  console.trace();
  console.warn(window.watchCount + ': watch', watch.get, ellapsed);
}else if (ellapsed > 1) {
  console.log(window.watchCount + ': watch', watch.get, ellapsed);
}
```
그러면 실제로 watch를 확인하는 횟수와 시간이 측정됩니다.  

그리고 $digest함수가 시작하는 부분에 아래의 코드를 집어넣습니다.  
``` js  
 if (this === $rootScope) {
      window.rootScopeDigestCount = window.rootScopeDigestCount + 1 || 1;
      console.trace('rootScope digest: ' + window.rootScopeDigestCount);
  }
```  
그러면 시간동안 $rootScope.$digest가 몇번을 돌았는지 확인할 수 있습니다.  
정확한 측정을 위해서는 app.run 부분에 일정 시간 이후의 window.watchCount를 console로 비교하기에 좋습니다.  

위의 코드를 제가 두레이 첫 화면에 적용시켜보았습니다.  

### 결과  

먼저 위 코드의 결과입니다.  
$rootScope.$digest 수: 23번  
전체 watch 수 : 24774번  

그리고 위에서 말씀드린 사항들을 찾아서 적용하였더니 아래와 같은 결과가 나왔습니다.  
$rootScope.$digest 수: 4번  
전체 watch 수 : 6246번  

이렇게 $rootScope.$digest 호출 수를 성공적으로 줄였습니다. 하지만 여기서 문제가 생겼는데 이렇게 개선을 했는데 하고 난 후의 첫페이지 로딩 속도는 오히려 느려진 것입니다. 

### 속도가 느려진 원인  

이 원인은 대부분의 $rootScope.$digest가 AJAX호출하는 pending 중에 호출된다는 데에 있었습니다. 그리고 이 때에 나중에 처리해야할 로직을 미리처리하는 효과가 나타나서 오히려 속도가 빠른 결과가 나타났습니다. 그래서 이부분을 해결하기 위해 각 뷰가 호출될 때에 $rootScope.$digest를 호출하게 했더니 속도가 약간 상승하는 효과가 있었습니다.    

## 결론  

$rootScope.$digest가 너무 많이 있으면 속도에 병목을 일으키지만 적절한 $rootScope.$digest는 속도를 오히려 높이는 효과가 나타납니다. 이 부분은 어플리케이션 별로 다르겠지만 적절한 $rootScope.$digest를 사용하게 해야할 것입니다.   