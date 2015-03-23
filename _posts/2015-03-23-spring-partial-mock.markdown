---
layout: article
title: "스프링 부분목킹"
date: 2015-03-23 11:54:13
image: 
  teaser: spring.png
categories: technology
---

# 스프링에서 부분목 하는 방법
스프링에서 메소드 하나가 너무 길어서 세부 private등의 메소드로 나누는데 이런 거대한 메소드를 테스트하려면 너무많은 시나리오들이 생깁니다.  
이럴 때에 부분목을 사용하면 보다 편리하게 테스트를 작성할 수 있습니다. 

## PowerMock
파워목이라는 것이 있습니다. 이 도구를 사용하면 테스트 하기 까다로운 static method나 private method 등의 메소드들을 mocking할 수 있습니다.

[powermock을 이용한 private 메소드 mocking](https://code.google.com/p/powermock/wiki/MockPrivate)

## Spy
Mockito에서 spy라는 방법이 있습니다. 자세한 방법은 아래에 있습니다.

[spy를 이용해서 부분목 사용](http://docs.mockito.googlecode.com/hg/latest/org/mockito/Mockito.html#spy)

## 참고자료  
[powermock](https://code.google.com/p/powermock/)  
[mockito](http://docs.mockito.googlecode.com/hg/latest/org/mockito/Mockito.html)  