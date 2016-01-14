---
layout: article
title: "[angularjs] - bootstrap"
date: 2015-06-28 14:07:13
image:
  teaser: angularjs.png
categories: technology
---

# 개요
angularjs를 사용하다가 보면 어떤식으로 angularjs가 실행되게 되는지 궁금할 때가 많이 있습니다.  
그래서 한 번 정리해 봤습니다.  

## 초기 설정
ng-app을 html태그에 명시해주고, script 태그에서 angularjs 코드 위치를 명시해줍니다.  
html에 ng-app을 명시할 경우 auto-bootstrap되어 angularjs를 사용할 수 있습니다.  

## 자동 초기화
angularjs 초기화는 DOMContentLoaded이벤트가 발생하거나 document.readyState가 'complete'로 설정되면 시작됩니다.

- 먼저 directive와 관련된 모듈을 불러옵니다.
- application의 injector를 만듭니다.
- ng-app을 root로 DOM compile을 시작합니다.

<figure>
	<a href="//kbs0327.github.io/blog/images/angularjs-concepts-startup.png"><img src="//kbs0327.github.io/blog/images/angularjs-concepts-startup.png"></a> 
	<figcaption>자동 부트스트랩 과정</figcaption>
</figure>

## 수동 초기화
angular.bootstrap이라는 함수를 통하여 수동초기화를 할 수 있습니다.  
이 함수를 이용하면 custom module을 정의할 수 있습니다.

## 참고자료  
[AngularJs 공식 홈페이지](https://angularjs.org/)
