---
layout: article
title: "css selector 우선순위"
date: 2015-06-14 14:07:13
image:
  teaser: css3.jpeg
categories: technology
---

# 개요
css를 적용시킬 때는 보통 css selector를 통해 해당 돔 엘리먼트를 가리키게 됩니다.  
그런데 동일한 돔 엘리먼트에 여러 css가 적용되어 있으면 우선순위에 따라 특정 CSS가 덮어쓰게 됩니다.  
그래서 CSS selector의 우선순위에 대해서 알아보겠습니다.  
  
## css selector 종류
 *: 전체 요소를 선택함  
 #id: 해당 id를 가지고 있는 요소를 선택  
 .class: 해당 클래스를 가지고 있는 요소를 선택  
 element: 해당 태그를 가지고 있는 요소를 선택  
 [attribute]: 해당 attribute를 가지고 있는 요소 선택  
 [attribute=value]: 해당 attribute의 값이 value인 요소를 선택  
 el1>el2: el1의 자식인 el2를 선택  
 el1 el2: el1의 자손인 el2를 선택  
 el1,el2: el1과 el2를 모두 선택  
 el1+el2: el1의 바로 뒤에 오는 요소인 el2를 선택  

## css selector의 우선순위
css요소의 우선순위는 먼저 !important를 가진 요소를 가장 최우선으로 봅니다.  
그리고 그 이후의 우선순위로는 W3C에서 게시한 내용으로 알 수 있습니다.  

### W3C 게시 내용
- a = 인라인 요소  
- b = id selector의 수  
- c = class selector 나 id를 제외한 attribute selector의 수  
- d = 요소의 이름(태그이름)이나 pseudo-요소(ex. :visited)의 수  
위의 내용에서 a > b > c > d로 보고 숫자는 높을 수록 높게 됩니다.  

W3C에서는 위의 내용의 예를 아래처럼 게시하고 있습니다.  
*             {}  /* a=0 b=0 c=0 d=0 -> specificity = 0,0,0,0 */  
li            {}  /* a=0 b=0 c=0 d=1 -> specificity = 0,0,0,1 */  
li:first-line {}  /* a=0 b=0 c=0 d=2 -> specificity = 0,0,0,2 */  
ul li         {}  /* a=0 b=0 c=0 d=2 -> specificity = 0,0,0,2 */  
ul ol+li      {}  /* a=0 b=0 c=0 d=3 -> specificity = 0,0,0,3 */  
h1 + *[rel=up]{}  /* a=0 b=0 c=1 d=1 -> specificity = 0,0,1,1 */  
ul ol li.red  {}  /* a=0 b=0 c=1 d=3 -> specificity = 0,0,1,3 */  
li.red.level  {}  /* a=0 b=0 c=2 d=1 -> specificity = 0,0,2,1 */  
#x34y         {}  /* a=0 b=1 c=0 d=0 -> specificity = 0,1,0,0 */  
style=""          /* a=1 b=0 c=0 d=0 -> specificity = 1,0,0,0 */  

## 참고자료  
[W3C specificity](http://www.w3.org/TR/CSS21/cascade.html#specificity)  
[CSS선택자와 우선순위 블로그](http://circlash.tistory.com/570)  
