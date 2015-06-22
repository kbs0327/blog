---
layout: article
title: "angularjs directive"
date: 2015-06-22 14:07:13
image:
  teaser: angularjs.png
categories: technology
---

# 개요
angularjs를 사용하면서 가장 배우기 힘들었던 기능이지만 가장 편리하게 쓰고 있는 기능 중에 directive객체가 있습니다.  
directive객체는 html 템플릿을 사용하여 그 템플릿을 통하여 사용할 수 있는 여러가지 함수들을 연결시켰습니다.  

## restrict
directive가 주입될 위치를 선정하는 방식입니다.  
e: element 이름  
a: attribute 이름  
c: class 이름  

## scope
scope는 디렉티브가 독자적인 scope를 가질 것인지를 결정합니다.  
기본값: true로 html이 들어있는 scope의 객체를 상속받습니다.  
false: 상속받지 않고 새로운 객체를 생성합니다.  
{}: 오브젝트를 사용하여 해당 오브젝트의 값들을 scope로 할당합니다.  

### 값 주입
directive에 값을 주입하는 방법에도 여러가지 방법이 있습니다.  
=: parameter의 값을 동일하게 적용시킵니다.  
&: reference를 주입받습니다.  

### link함수
directive가 만들어진 후에 그 템플릿을 이용하는 함수들을 모아 놓은 곳으로 directive에 해당하는 로직들을 모아놓습니다.  

### template과 templateUrl
연결할 html을 설정하는 값으로  
template는 해당 html값을 주입받고  
templateUrl은 html이 있는 링크를 연결시킵니다.  

## 참고자료  
[AngularJs 공식 홈페이지](https://angularjs.org/)
