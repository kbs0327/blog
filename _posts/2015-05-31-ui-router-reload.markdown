---
layout: article
title: "Angularjs ui-router reload하기"
date: 2015-05-31 14:07:13
image:
  teaser: angularjs.png
categories: technology
---

# 개요
Angularjs에서는 화면을 구성할 때에 ui-router를 사용해서 화면의 일부만 다시 그리는 방법을 사용하고 있습니다.  
그런데 ui-router를 사용하다보면 state를 이동시켰는데 화면이 갱신이 안되는 현상이 종종 일어납니다.  
그 문제에 대해서 알아보겠습니다.  

## ui-router란
Angularjs에서는 화면을 state라는 상태를 기준으로 화면 일부만 다시그릴 수 있게 지원해 줍니다.  
또한 state를 url과 연결해서 관리할 수 있어서 원하는 화면의 정보는 url을 통하여 해당 화면으로 이동시킬 수 있고, state를 통해서만 이동시키게도 할 수 있습니다.  
이렇게 state를 이동할 때 외부에서는 url을 통하여 이동하겠지만 내부에서 링크를 통하여 이동할 때에는 $state.go라는 함수나 html에서는 ui-sref=""이라는 함수를 이용하여 이동할 수 있습니다.  

### $state.go
함수형식: go(to, params, options)
이 함수는 state를 이동하게 하는 함수로써 to에는 이동할 state의 name을 적고 원하는 params를 object형태로 입력합니다.  
그리고 원하는 options를 또한 object형태로 입력시켜서 사용할 수 있습니다.  

#### 매개변수들
to: 이동할 state의 name을 적으면 그 state로 이동할 수 있습니다. to에는 특별한 character를 적어서 state의 name을 줄여서 적을 수 있습니다.  
- '.': 현재의 state를 가리킵니다. 응용해서 '.child'로 쓰면 현재 state를 기준으로 child를 추가한 state로 이동합니다.  
- '^': 부모의 state를 가리킵니다. 응용해서 부모의 부모 state를 가리키고 싶으면 '^.^'로 쓰면 부모의 부모 state로 이동할 수 있습니다.  
params: 이동할 state에 parameter들을 저장할 수 있습니다. 형식은 object형식으로 저장하며, state의 url을 설정하면 '?key=value'형태로 나타낼 수 있습니다.  
- $stateParams: 참고로 params에 적은 object는 이동 후에 $stateParams에 저장됩니다.  
options: state를 이동할 때에 다양한 옵션을 줘서 다르게 동작할 수 있게 합니다. 이번에 살펴볼 것은 reload옵션입니다.  

### ui-sref
형식: ui-sref=" {{ state name }} ( {{ params 객체 }} )"  
ui-sref는 html에 attribute에 위의 형식으로 넣어서 동작할 수 있습니다.  

ui-sref에 option을 주는 방법으로 ui-sref-opts=" {{ option 객체 }} " attribute를 줄 수 있습니다.  

## state이동시 reload문제
state를 이동할 때에 url이 동일하면 이동을 해도 화면을 reload 하지 않는 문제가 있습니다.  
그래서 state를 이동할 때에 {reload: true}옵션을 주고 이동하게 됩니다.  
하지만 이렇게 이동을 하면 전체 페이지가 reload되는 문제가 있습니다.  

그래서 ui-router v0.2.5부터는 전체 view가 아닌 부분만 reload하게 할 수 있습니다.  
그 방법은 {reload: {{ state name }} } 형식으로 호출하는 것입니다.  

state name은 갱신하고 싶은 대상의 state name을 적고, 해당 state부터 하위의 state까지 모두 reload가 됩니다.  

## 참고자료
[ui-router 공식사이트](http://angular-ui.github.io/ui-router/site/#/api/ui.router)
