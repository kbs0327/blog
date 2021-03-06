---
layout: article
title:  "웹접근성 정리"
date:   2015-03-03 13:52:13
image:
  teaser: accessibility.png
categories: technology
---

## 웹접근성이란
 어떠한 사용자(장애인, 노인 등), 어떠한 기술환경에서도 전문적인 능력 없이 웹 사이트에서 제공하는 모든 정보에 접근할 수 있도록 보장하는 것.  

## 장애 환경에 대한 이해

<figure>
  <a href="http://nuli.navercorp.com/data/accessibility/img_a11y_environment.png"><img src="http://nuli.navercorp.com/data/accessibility/img_a11y_environment.png"></a>
	<figcaption>장애환경 인포그래픽</figcaption>
</figure>

## NWCAG
 NHN Web Content Accessibility Guidelines(이하 NWCAG) 1.0은 NHN에서 제공하는 웹 콘텐츠의 접근성을 보장하기 위한 목적으로 만들어졌다.

### 대체 콘텐츠를 제공한다.  

1. 텍스트가 아닌 콘텐츠는 원래 제공하고자 하는 정보 그대로 대체 텍스트를 제공한다.  
2. 정보 전달을 위한 동영상은 캡션이나 원고를 제공한다.  
3. 캡차 사용 시 대체 가능한 수단을 제공한다.  

### 어떠한 경우에도 정보나 구조의 손실이 없어야 한다.  

1. 색상, 크기, 방향, 위치, 음향 효과 만으로 정보를 전달하지 않는다.  
2. W3C 명세에 맞게 마크업한다.  
 - 문법 오류를 방지하여 브라우저가 잘못 해석하는 것을 막는 것  
3. 의미에 맞는 요소와 구조로 마크업한다.  
4. 콘텐츠는 논리적인 순서에 맞게 제공한다.  
5. 웹 브라우저가 기본적으로 지원하지 않는 객체는 대체 콘텐츠를 함께 제공한다.  
6. 단순 열람을 위한 문서는 뷰어를 반드시 제공한다.  
7. 웹 브라우징 도구의 기본 인터페이스를 제한하지 않는다.  

### 읽고 이해하기 쉬운 환경을 보장한다.  

1. 주 언어를 명시한다.  
2. 자동으로 재생되는 3초 이상의 음성 정보는 콘텐츠 시작 부분에서 사용자가 제어할 수 있게 한다.  
3. 콘텐츠와 배경간의 명도 대비는 4.5:1 이상이 되게 한다.  

### 충분한 시간을 보장한다.  

1. 시간 제한이 있는 콘텐츠는 지양하며, 서비스 특성상 제공되어야 할 경우, 사용자에게 미리 공지한다.  
2. 자동으로 갱신되는 콘텐츠는 사용자가 제어할 수 있게 한다.  

### 발작을 유발할 수 있는 콘텐츠는 제공하지 않는다.  

1. 1초에 3회 이상 깜빡거림이 있는 콘텐츠는 제공하지 않는다.  

### 키보드 접근성을 보장한다.  

1. 마우스 이벤트로 동작하는 기능은 키보드로도 사용이 가능해야 한다.  
2. 키보드 포커스는 논리적인 순서로 이동해야 한다.  
3. 포커스를 받은 영역은 시각적으로 구분이 가능해야 한다.  

### 쉽고 빠른 내비게이션 환경을 보장한다.  

1. 페이지, 프레임, 콘텐츠 블록에는 각 영역을 설명하는 제목을 제공한다.  
2. 반복되는 콘텐츠를 건너뛸 수 있는 링크를 제공한다.  
3. 링크 텍스트는 명확하게 제공한다.  

### 사용자 오류를 예방한다.  

1. 사용자가 의도하지 않은 기능은 실행되지 않게 한다.  
2. 폼 콘트롤 요소의 역할을 명확하게 이해할 수 있는 레이블을 제공한다.  
3. 잘못된 값이 입력되었을 경우, 사용자에게 알려주고 수정할 수 있게 한다.  
4. 유효한 값을 입력할 수 있도록 안내 텍스트를 미리 제공한다.  
5. 세션 만료 시, 재 인증 후에도 이전 동작을 무리 없이 수행할 수 있게 한다.  

## 참고자료  
주요 참고 [널리 웹접근성의 이해](http://nuli.navercorp.com/sharing/a11y)  
웹접근성 정의 [한국정보화진흥원 연구소](http://wah.or.kr)  
