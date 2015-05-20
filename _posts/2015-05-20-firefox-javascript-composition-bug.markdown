---
layout: article
title: "파이어폭스에서 한글조합 문제 해결"
date: 2015-05-20 14:07:13
image:
  teaser: firefox.jpeg
categories: technology
---

# 개요
파이어폭스에서 한글을 조합할 때는 'keydown'이벤트가 발생하지 않지만 다른 브라우저에서는 모두 'keydown'이벤트를 발생합니다.

## 문제 원인
>W3C표준에는 다음과 같이 명시되어 있습니다.

>**During the composition session, all keydown and keyup events may be suppressed.**

>하지만 파이어폭스만 supress하고 있고, 다른 브라우저들은 supress하고 있지 않습니다.
>(즉, 파이어폭스만 표준을 따르고 있는 상황입니다.)

>이러한 문제로 'keydown'이벤트를 이용하여 자동완성하는 component들은 파이어폭스에서 제대로 작동하지 않게 됩니다.

## 해결방법
파이어폭스에서는 대신 한글 조합중에 'keydown'이벤트 대신 'keypress'이벤트가 발생합니다.
그래서 이것을 이용해서 문제를 해결할 수 있습니다.

다은은 한글조합중에 엔터나 탭키를 눌렀을 때의 이벤트를 처리하는 방법입니다.
{% highlight js %}
element.bind("keypress", function (event) {
	var keyCode = event.which || event.keyCode;
   	if(keyCode === 13 || keyCode === 9) { // 13: enter, 9: tab
    // code
    }
}
{% endhighlight %}

저는 라이브러리 컴포넌트를 사용할 때 문제가 생겼는데 이 컴포넌트는 'keydown'이벤트로만 동작하는 컴포넌트였습니다.
그래서 'keypress'이벤트가 발생했을 때에 'keydown'이벤트를 강제로 발생시켰습니다.

## 'keypress'와 'keydown'이벤트가 같이 호출되지는 않는가?
저처럼 'keypress'이벤트가 발생하면 'keydown'이벤트를 강제로 발생시키면 위와 같은 의문점이 들 수 있습니다.
이에 대해서 MDN에서는 이렇게 설명하고 있습니다.

There are keydown, keypress, and keyup events. For most keys, Gecko dispatches a sequence of key events like this:

1. When the key is first depressed, the keydown event is sent.
2. If the key is not a modifier key, the keypress event is sent.
3. When the user releases the key, the keyup event is sent.

즉, 적어도 MDN의 Gecko엔진을 사용하는 파이어폭스에서는 'keydown'과 'keypress'이벤트는 동시에 발생하지 않는다는 것입니다.

그러므로 브라우저가 파이어폭스라고만 명시한다면 'keydown'이벤트와 'keypress'이벤트 중 하나만 발생한다고 할 수 있고,
따라서 위에서 제가 사용한 방법을 사용할 수 있습니다.

## 참고자료
[MDN 공식사이트 키보드 이벤트](https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent)
