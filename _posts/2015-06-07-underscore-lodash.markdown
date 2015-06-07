---
layout: article
title: "Underscore와 Loadash 차이"
date: 2015-06-07 14:07:13
image:
  teaser: lodash.jpg
categories: technology
---

# 개요
제가 자바스크립트로 개발을 하면서 많이 들어본 라이브러리 중에 underscore와 lodash라이브러리가 있습니다.  
그런데 lodash나 underscore나 사용예를 보면 '_.func()'형식으로 형식이 많이 비슷합니다.  
그래서 이 두개의 라이브러리가 어떻게 다른지 알아보았습니다.  

## underscore란?
underscore는 자바스크립트의 함수형 프로그래밍에 초점을 맞춘 유틸리티성 라이브러리로 Backbone에 의존성을 가지고 있습니다.  
underscore는 Backbone을 만든 'Jeremy Ashkenas'가 만들었습니다.  

## lodash란?
lodash 또한 자바스릅트 유틸리티성 라이브러리로 크로스환경에 더 안전성을 주고자 만들어졌습니다.  
그리고 현재는 underscore의 superset의 형태로 함수, 문서, 유닛 테스트, 성능이 더 뛰어나다고 lodash의 개발자인 'John-David Dalton'은 말하고 있습니다.  

## 차이점
lodash는 확인해보면 정말 underscore의 superset이라고 할 수 있습니다.  
먼저 underscore의 API를 구현하였으며(내부 구현은 차이가 있습니다.) underscore에는 없는 함수들을 추가로 개발하였습니다.  
<iframe width="100%" height="420" src="http://cloud.highcharts.com/embed/utusen/3" style="border: 0px;"></iframe>
위처럼 성능을 확인해보면 underscore의 API보다 lodash의 API가 대체로 더 성능이 좋은 것을 확인할 수 있습니다.  

## 어떤 것을 써야하는가?
만약 NodeJS를 사용하는 유저라면 Lodash를 사용하는 것이 더 효율적으로 보입니다.

## 참고자료  
[underscore 공식사이트](http://underscorejs.org/)  
[lodash 공식사이트](https://lodash.com/)  
[underscore vs lodash](http://benmccormick.org/2014/11/12/underscore-vs-lodash/)  
[underscore와 lodash의 차이에 대한 staskoverflow 글](http://stackoverflow.com/questions/13789618/differences-between-lodash-and-underscore)  
