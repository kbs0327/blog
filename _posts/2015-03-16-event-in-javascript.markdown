---
layout: article
title: "이벤트"
date: 2015-03-16 13:34:13
image:
  teaser: javascript_book.jpg
categories: book  
---

# 이벤트

## 개요
자바스크립트와 HTML의 상호작용은 이벤트에의해 처리됩니다.  
이벤트는 리스너(핸들러)로 추적하며 리스너는 이벤트 호출시에만 실행됩니다.  
이 모델을 옵저버 패턴이라고 합니다.  
DOM 레벨2에서 이벤트의 핵심을 구현했고, 브라우저 전용 이벤트시스템만 사용한 브라우저는 인터넷익스플로러8이 마지막입니다.  

## 이벤트 흐름
이벤트의 흐름은 페이지의 어느부분이 특정이벤트를 소유해야 할 것인가? 라는 질문에서 시작됬습니다.  
이에 대한 이벤트 흐름은 인터넷 익스플로러와 넷스케이프에서 다른방식으로 접근했습니다.  

- 인터넷 익스플로러: 이벤트 버블링
- 넷스케이프 커뮤니케이터: 이벤트 캡처링  

### 이벤트 버블링
이벤트의 가장 안쪽부터 거슬러 올라가는 흐름

{% highlight html %}
<!DOCTYPE html>
<html>
	<head>
		<title>Event Bubbling Example</title>
	</head>
	<body>
		<div id="myDiv">
			Click Me
		</div>
	</body>
</html>
{% endhighlight %}
페이지에서 div요소를 클릭하면 이벤트는 아래 순서대로 발생합니다.  

1. div
2. body
3. html
4. document

예외  
IE5.5: html요소를 건너 뛰고 body -> document로 올라갑니다.  
IE9, 파이어폭스, 크롬, 사파리: 이벤트가 window객체까지 올라감  

### 이벤트 캡처링
최상위 노드에서 처음으로 이벤트가 발생하고 가장 명시적인 곳이 마지막에 발생합니다.  
위의 소스를 실행하면  

1. document
2. html
3. body
4. div

순서로 이벤트가 발생합니다.  
이벤트 캡처링은 현재 IE9, 사파리, 크롬, 오페라, 파이어폭스에서 지원합니다.  
이벤트는 window에서 시작합니다.  
오래된 브라우저에서는 지원을 안하므로 이벤트 버블링을 주로 쓰고 이벤트 캡처링은 특별상황에서만 사용해야합니다.  

### DOM 이벤트 흐름
DOM 레벨2 이벤트에서 정의한 이벤트 흐름에서는  

- 이벤트 캡처링 단계
- 타깃 단계
- 이벤트 버블링 단계

즉, 다음의 순서로 이벤트가 발생합니다.

1. document
2. html
3. body
4. div
5. body
6. html
7. document

DOM레벨2 이벤트 명세에서는 타깃에서 이벤트를 호출할 시에 버블링 단계에 속하는 것으로 간주합니다.  
하지만, IE9와 사파리, 크롬, 파이어폭스, 오페라9.5 및 이후버전은 캡처링 단계에서도 이벤트 타깃에 전달됩니다.  
즉, 타깃에서 이벤트를 2번 작업할 수 있습니다.  

## 이벤트 핸들러
이벤트핸들러(이벤트리스너): 이벤트에 응답하여 호출되는 함수  

### HTML 이벤트 핸들러
HTML속성에 할당하여 실행할 수 있습니다.  

{% highlight html %}
<input type="button" value="Click Me" onclick="alert('Clicked')"/>
{% endhighlight %}

이 코드에서는 자바스크립트도 HTML요소이므로 HTML이스케이프하여 표현해야 합니다.  
HTML 이벤트 핸들러는 인라인도 가능하지만 아래처럼 표현하는 것도 가능합니다.  

{% highlight html %}
<script type="text/javascript">
function showMessage() {
	alert("Hello world");
}
</script>
<input type="button" value="Click Me" onclick="showMessage()"/>
{% endhighlight %}

이런 방식으로 할당되면 event라는 특별한 로컬변수가 생기고 이는 이벤트 객체에 해당합니다.  
this값은 이벤트 타깃 요소와 일치합니다.

{% highlight html %}
<input type="button" value="Click Me" onclick="alert(event.type)">	<!-- click --> 
<input type="button" value="Click Me" onclick="alert(this.type)">	<!-- click Me --> 
{% endhighlight %}

아래와 같이 스코프 체인 확장이 일어납니다.

{% highlight js %}
function() {
	with(document) {
		with(this) {
			// 속성값
		}
	}
}
{% endhighlight %}

그래서 아래와같이 자신의 프로퍼티에 쉽게 접근할 수 있습니다.

{% highlight html %}
<input type="button" value="Click Me" onclick="alert(value)">	<!-- click Me --> 
{% endhighlight %}

요소가 폼 input요소인 경우 스코프체인에 부모인 폼 요소도 포합됩니다.  

{% highlight js %}
function() {
	with(document) {
		with(this.form) {
			with(this) {
				// 속성값
			}
		}
	}
}
{% endhighlight %}

이렇게 확장되는 속성대문에 이벤트 핸들러는 형제 멤버에 접근하기 쉽습니다.

{% highlight html %}
<form method="post">
	<input type="text" name="username" value="">
	<input type="button" value="Echo Username" onclick="alert(username.value)">
</form>
{% endhighlight %}

HTML에 이벤트 핸들러 할당하는 방법의 단점들  

1.핸들러가 정의되지 않은 상태에서 버튼을 누르면 에러가 발생할 때도 있습니다.  
그래서 아래처럼 에러를 조용히 처리하게 합니다.  

{% highlight html %}
<input type="button" value="Click Me" onclick="try{showMessage();} catch(ex){}">
{% endhighlight %}

2.이벤트 핸들러 함수의 스코프 체인 확장 결과가 브라우저마다 다름  
3.HTML과 자바스크립트가 너무 단단히 묶임

### DOM 레벨0 이벤트 핸들러
이벤트핸들러를 할당하는 전통적인 방법은 이벤트 핸들러 프로퍼티에 함수를 할당하는 것입니다.

{% highlight js %}
document.getElementById("myBtn");
btn.onclick = function () {
	alert("Clicked");
};
{% endhighlight %}

DOM 레벨0방법으로 이벤트 핸들러를 할당하면 해당 요소의 메서드로 간주됩니다.  
이 이벤트 핸들러는 요소의 스코프에서 실행되며 this는 요소 자체입니다.  

{% highlight js %}
document.getElementById("myBtn");
btn.onclick = function () {
	alert(this.id);	// myBtn
};
{% endhighlight %}

이벤트 버블링 단계에 실행되도록 의도한 것입니다.  
이를 해제하기 위해서는 아래와 같이 하면 됩니다.

{% highlight js %}
btn.onclick = null;
{% endhighlight %}

### DOM 레벨2 이벤트 핸들러
DOM 레벨2 이벤트에서는 담당 메서드를 도입했습니다.  
addEventListener: 이벤트 할당  
removeEventListener: 이벤트 제거  

{% highlight js %}
var btn = document.getElementById("myBtn");
btn.addEventListener("click", function () {
	alert(this.id);	//myBtn
}, false);
{% endhighlight %}

이벤트는 마지막 매개변수가 false여서 버블링 단계에 실행됩니다.  
이벤트는 요소의 스코프에서 실행됩니다. 또한 이벤트는 여러개 존재할 수 있습니다.  
  
{% highlight js %}
var btn = document.getElementById("myBtn");
btn.addEventListener("click", function () {
	alert(this.id);	//myBtn
}, false);
btn.addEventListener("click", function () {
	alert("Hello world");	// Hello world
}, false);
{% endhighlight %}

이벤트를 제거할 때는 핸들러를 추가할 때와 같은 매개변수로 removeEventListener를 호출해야합니다.  
즉, 추가한 익명함수는 제거할 수 없습니다.

{% highlight js %}
var btn = document.getElementById("myBtn");
btn.addEventListener("click", function () {
	alert(this.id);	//myBtn
}, false);

btn.removeEventListener("click", function () { // 동작하지 않음
	alert(this.id);
}, false);
{% endhighlight %}

이는 아래와 같이 써야 동작합니다.

{% highlight js %}
var btn = document.getElementById("myBtn");
var handler = function () {
	alert(this.id);
};

btn.addEventListener("click", handler, false);
btn.removeEventListener("click", handler, false);
{% endhighlight %}

### 인터넷 익스플로러 이벤트 핸들러
인터넷 익스플로러도 DOM표준과 비슷한 메서드를 구현했습니다.
attachEvent: 이벤트 할당
detachEvent: 이벤트 제거

{% highlight js %}
var btn = document.getElementById("myBtn");
btn.attachEvent("onclick", function() {
	alert("Clicked");
});
{% endhighlight %}

attachEvent로 등록한 이벤트 핸들러는 전역 컨텍스트에서 실행됩니다.

{% highlight js %}
var btn = document.getElementById("myBtn");
btn.attachEvent("onclick", function() {
	alert(this === window); // true
});
{% endhighlight %}

요소에 여러 이벤트를 추가할 수 있습니다.  
하지만 순서는 반대로 호출됩니다.

{% highlight js %}
var btn = document.getElementById("myBtn");
btn.attachEvent("onclick", function() {
	alert("Clicked");
});
btn.attachEvent("onclick", function() {
	alert("Hello world!");
});
{% endhighlight %}

detachEvent로 등록한 이벤트를 삭제할 수 있습니다.(익명함수 제거 안됩니다.)

### 크로스 브라우저 이벤트 핸들러
가능한한 많은 브라우저에서 동작하게 하려면 버블링 단계에서 이벤트를 동작시켜야 합니다.  
크로스 브라우저 이벤트 핸들러 EventUtil을 만듭니다.  

{% highlight js %}
var EventUtil = {
	addHandler: function (element, type, handler) {
		if (element.addEventListener) {
			element.addEventListener(type, handler, false);
		} else if (element.attachEvent) {
			element.attachEvent("on" + type, handler);
		} else {
			element["on" + type] = handler;
		}
	},
	removeHandler: function (element, type, handler) {
		if (element.removeEventListener) {
			element.removeEventListener(type, handler, false);
		} else if (element.attachEvent) {
			element.attachEvent("on" + type, handler);
		} else {
			element["on" + type] = null;
		}
	}
}

var btn = document.getElementById("myBtn");
var handler = function () {
	alert("Clicked");
};

EventUtil.addHandler(btn, "click", handler);

EventUtil.removeHandler(btn, "click", handler);
{% endhighlight %}

## event 객체
모든 브라우저가 지원하지만 세부사항까지 같지는 않습니다.  
세부사항은 아래와 같습니다.

- 발생한 요소  
- 이벤트 타입  
- 이벤트 관련 다른 데이트  
 - 마우스 위치정보  
 - 키보드 키코드  

### DOM event 객체
DOM표준을 준수하는 브라우저에서 이벤트 핸들러에 전달되는 매개변수는 event객체 하나뿐입니다.  
다음과 같이 사용할 수 있습니다.

{% highlight js %}
var btn = document.getElementById("myBtn");
btn.onclick = function (event) {
	alert(event.type);	//click
};

btn.addEventListener("click", function (event) {
	alert(event.type);	//click
}, false);
{% endhighlight %}

이벤트와 관련된 프로퍼티와 메서드는 [링크](https://developer.mozilla.org/en-US/docs/Web/API/Event)를 참조하면 됩니다.  
다음 코드를 살펴보면 this와 currentTarget, target의 상관관계를 알 수 있습니다.  

{% highlight js %}
var btn = document.getElementById("myBtn");
btn.onclick = function (event) {
	alert(event.currentTarget === this);	// true
	alert(event.target === this);			// true
}

document.body.onclick = function (event) {
	alert(event.currentTarget === this);	// true
	alert(event.target === this);			// true
	alert(event.target === document.getElementById("myBtn"));	// true
}
{% endhighlight %}

이 프로퍼티들을 이용해서 하나의 함수에서 여러 이벤트를 처리하게 할 수 있습니다.  

{% highlight js %}
var btn = document.getElementById("myBtn");
var handler = function (event) {
	switch (event.type) {
	case "click":
		alert("Clicked");
		break;
		
	case "mouseover":
		event.target.style.backgroundColor = "red";
		break;
		
	case "mouseout":
		event.target.style.backgroundColor = "";
		break;
	}
};

btn.onclick = handler;
btn.onmouseover = handler;
btn.onmouseout = handler;
{% endhighlight %}

preventDefault()로 이벤트를 취소할 수 있습니다.(단, cancelable 프로퍼티가 true여야함)  
그리고 stopPropagation()메서드는 이벤트 흐름을 즉시 멈춰서 캡처링 버블링을 취소합니다.  
eventPhase 프로퍼티: 현재 어느단계에 있는지 알려줌  
1: 캡처단계  
2: 타깃 호출  
3: 버블링  

{% highlight js %}
// preventDefault 실습
var link = document.getElementById("myLink");
link.onclick = function (event) {
	event.preventDefault();
};

// stopPropagation 실습
var btn = document.getElementById("myBtn");
btn.onclick = function (event) {
	alert("Clicked");
	event.stopPropagation();
};

document.body.onclick = function (event) {
	alert("Body clicked");
};

// eventPhase 실습
var btn = document.getElementById("myBtn");
btn.onclick = function (event) {
	alert(event.eventPhase);	// 2
};

document.body.addEventListener("click", function (event) {
	alert(event.eventPhase);	// 1
}, false);

document.body.onclick = function (event) {
	alert(event.eventPhase);	// 3
};
{% endhighlight %}

### 인터넷 익스플로러의 event 객체
이벤트 핸들러를 어떻게 할당했느냐에 따라 다르게 접근합니다.  
DOM 레벨0 접근법으로 할당하면 event객체는 오직 window의 프로퍼티로 존재합니다.  
attachEvent로 할당하면 event객체는 함수의 유일한 매개변수로 전달됩니다.  
HTML 속성에서 할당하면 event 변수에 저장됩니다.  
인터넷 익스플로러의 공통 프로퍼티와 메서드는 [링크](https://msdn.microsoft.com/en-us/library/ie/ms535863%28v=vs.85%29.aspx)를 확인하기 바랍니다.  

{% highlight js %}
// DOM0 접근법
var btn = document.getElementById("myBtn");
btn.onclick = function () {
	var event = window.event;
	alert(event.type);	// click
};

// attachEvent 접근법
var btn = document.getElementById("myBtn");
btn.attachEvent("onclick", function (event) {
	alert(event.type);	//click
});
{% endhighlight %}

{% highlight html %}
// HTML 접근법
<input type="button" value="Click Me" onclick="alert(event.type)">			<!-- click -->	
<input type="button" value="Click Me" onclick="alert(window.event.type)">	<!-- click이므로 event 객체는 window변수에 저장됨-->
{% endhighlight %}

this도 상황에 따라 다르므로 event.srcElement를 사용하는 편이 좋습니다.  
returnValue 프로퍼티는 false를 입력하면 기본동작이 취소됩니다.  
cancelBubble 프로퍼티는 true를 입력하면 버블링이 취소됩니다.

{% highlight js %}
// this와 event.srcElement
var btn = document.getElementById("myBtn");
btn.onclick = function () {
	alert(window.event.srcEelement === this);	// true
};

btn.attachEvent("onclick", function (event) {
	alert(event.srcElement === this);			// false
});

// returnValue 실습
var link = document.getElementById("myLink");
link.onclick = function () {
	window.event.returnValue = false;
};

// cancelBubble 실습
var btn = document.getElementById("myBtn");
btn.onclick = function () {
	alert("Clicked");
	window.event.cancelBubble = true;
};

document.body.onclick = function () {
	alert("Body clicked");
};
{% endhighlight %}

### 크로스 브라우저 이벤트 객체
DOM 표준과 IE의 event객체를 크로스브라우저 솔루션으로 구현할 수 있습니다.  

{% highlight js %}
var EventUtil = {
	addHandler: function (element, type, handler) {
		if (element.addEventListener) {
			element.addEventListener(type, handler, false);
		} else if (element.attachEvent) {
			element.attachEvent("on" + type, handler);
		} else {
			element["on" + type] = handler;
		}
	},
	getEvent: function (event) {
		return event ? event : window.event;
	},
	
	getTarget: function (event) {
		return event.target || event.srcElement;
	},
	
	preventDefault: function (event) {
		if (event.preventDefault) {
			event.preventDefault();
		} else {
			event.returnValue = false;
		}
	},
	
	removeHandler: function (element, type, handler) {
		if (element.removeEventListener) {
			element.removeEventListener(type, handler, false);
		} else if (element.attachEvent) {
			element.attachEvent("on" + type, handler);
		} else {
			element["on" + type] = null;
		}
	},
	
	stopPropagation: function (event) {
		if (event.stopPropagation) {
			event.stopPropagation();
		} else {
			event.cancelBubble = true;
		}
	}
};
{% endhighlight %}

## 이벤트 타입
- UI이벤트는 브라우저 이벤트이며 BOM과 상호작용합니다
- 포커스 이벤트는 포커스를 얻거나 잃을때 발생합니다
- 마우스 이벤트는 마우스로 어떤 동작을 취할 때 발생합니다
- 휠 이벤트는 마우스 휠이나 비슷한 장치를 사용할 때 발생합니다
- 텍스트 이벤트는 문서에 텍스트를 입력할 때 발생합니다
- 키보드 이벤트는 키보드로 동작을 취할 때 발생합니다
- 구성 이벤트는 IME를 통해 문자를 입력할 때 발생합니다
- 변경 이벤트는 DOM구조가 바뀔 때 발생합니다
- 이름변경이벤트는 폐기되었습니다

### UI 이벤트
사용자와 직접 관련이 없으며 DOM명세 이전에 만들어져서 하위 호환성 때문에 남겨졌습니다.  

- DOMActive
 - 사용자가 마우스나 키보드로 요소를 활성화했을 때 발생합니다.  
 - DOM레벨3에서 폐기되었지만 파이어폭스2이상 크롬에서 지원합니다.  
- load
 - 페이지를 완전히 불러왔을 때 window에서
 - 모든 프레임을 완전히 불러왔을 때 프레임셋에서
 - img나 object요소에서 발생합니다.
- unload
 - 페이지를 완전히 종료했을 때 window에서
 - 모든프레임을 완전히 종료했을 때 프레임셋에서
 - 객체를 완전히 종료했을 때 object요소에서 발생합니다.
- abort
 - object요소의 콘텐츠를 완전히 내려받기 전에 취소했을 때 발생합니다.
- error
 - 자바스크립트 에러가 발생했을 때 window에서
 - 이미지를 불러올 수 없을 때 해당 img에서
 - object 요소를 불러올 수 없을 때 해당 프레임셋에서 발생합니다.
- select
 - 사용자가 input이나 textarea에서 글자를 선택할 때 발생합니다.
- resize
 - window나 프레임의 크기를 바꿀 때 발생합니다.
- scroll
 - 스크롤할 때 발생합니다.

위의 HTML이벤트는 DOMActivate를 제외하면 DOM 레벨2 이벤트의 HTML 이벤트 그룹에 속합니다.

{% highlight js %}
// HTML 이벤트가 DOM 레벨2 이벤트 지원
var isSupported = document.implementation.hasFeature("HTMLEvents", "2.0");

// HTML 이벤트가 DOM 레벨3 이벤트 지원
var isSupported = document.implementation.hasFeature("HTMLEvents", "3.0");
{% endhighlight %}

#### load 이벤트
window의 load이벤트: 이미지나 자바스크립트 파일, CSS파일 같은 외부자원을 포함해서 모든 페이지가 불러왔을 때 발생합니다.

{% highlight js %}
EventUtil.addHandler(window, "load", function (event) {
	alert("Loaded!");
});
{% endhighlight %}

{% highlight html %}
<!DOCTYPE html>
<html>
	<head>
		<title>Load Event Example</title>
	</head>
	<body onload="alert('Loaded!')">
		
	</body>
</html>
{% endhighlight %}

매개변수인 event값  

- DOM준수: event.target - document  
- IE 8미만: scrElement프로퍼티를 설정하지 않습니다.  

window에서 발생하는 이벤트는 body요소의 속성으로 할당 가능합니다.
하지만 가능한 한 자바스크립트 표현식을 쓰기 바랍니다.

이미지의 load이벤트: 이미지를 다 불러왔을 때 발생합니다.
요소에 src프로퍼티를 설정하는 즉시 이미지를 내려받습니다.

{% highlight html %}
<img src="smile.gif" onload="alert('Image loaded.')">
{% endhighlight %}

{% highlight js %}
var image = document.getElementById("myImage");
EventUtil.addHandler(image, "load", function (event) {
	event = EventUtil.getEvent(event);
	alert(EventUtil.getTarget(event).src);
});

EventUtil.addHandler(window, "load", function (event) {
	var image = document.createElement("img");
	EventUtil.addHandler(image, "load", function (event) {
		event = EventUtil.getEvent(event);
		alert(EventUtil.getTarget(event).src);
	});
	document.body.appendChild(image);
	image.src = "smile.gif";	// 이 순간 load 시작
});

EventUtil.addHandler(window, "load", function () {
	var image = new Image();
	EventUtil.addHandler(image, "load", function (event) {
		alert("Image loaded!");
	});
	image.src = "smile.gif";
});
{% endhighlight %}

비표준방식
IE9이상, 파이어폭스, 오페라, 크롬, 사파리3이상 - script load이벤트
IE와 오페라 - css load이벤트
두 이벤트는 문서에 요소가 추가된 이후에 전송이 시작됩니다.

{% highlight js %}
// script load 이벤트
EventUtil.addHandler(window, "load", function () {
	var script = document.createElement("script");
	script.type = "text/javascript";
	EventUtil.addHandler(script, "load", function (event) {
		alert("Loaded");
	});
	script.scr = "example.js";
	document.body.appenChild(script);
});

// css load 이벤트
EventUtil.addHandler(window, "load", function () {
	var link = document.body.createElement("link");
	link.type = "text/css";
	link.rel = "stylesheet";
	EventUtil.addHandler(link, "load", function (event) {
		alert("css loaded");
	});
	link.href = "example.css";
	document.getElementsByTagName("head")[0].appendChild(link);
});
{% endhighlight %}

#### unload 이벤트
문서를 완전히 닫을 때 발생하며, 메모리 누수 방지 목적으로 사용합니다.  
이 이벤트는 모든 것이 해제될 때 발생하므로 페이지에 존재하던 객체를 전부 사용할 수 없습니다.
event 프로퍼티  

- DOM준수: target - document  
- IE8이전: srcElement 지원 안합니다.  

{% highlight html %}
<!DOCTYPE html>
<html>
	<head>
		<title>Unload Event Example</title>
	</head>
	<body onload="alert('Unloaded!')">
		
	</body>
</html>
{% endhighlight %}

#### resize 이벤트
브라우저 창의 크기를 변경하면 발생합니다.  
body요소에 onresize핸들러를 사용해도 되지만 자바스크립트 접근법을 추천합니다.  
브라우저별 특징
- IE, 사파리, 크롬 오페라: 1픽셀이라도 바뀌면 발생하며 계속 발생함
- 파이어폭스: 창 크기 조절을 멈추는 시점에 발생

{% highlight js %}
EventUtil.addHandler(window, "resize", function (event) {
	alert("Resize");
});
{% endhighlight %}

#### scroll 이벤트
{% highlight js %}
EventUtil.addHandler(window, "scroll", function (event) {
	if(document.compatMode == "CSS1Compat") {
		alert(document.documentElement.scrollTop);
	} else {
		alert(document.body.scrollTop);
	}
});
{% endhighlight %}

문서를 스크롤하는 동안 반복 실행됨

### Focus 이벤트
포커스를 받거나 잃을 때 발생합니다.

- blur: 요소가 포커스를 잃을 때 발생합니다.(버블링X)  
- DOMFocusIn: 요소가 포커스 받을 때 발생합니다.(버블링O, 오페라만)  
- DOMFocusOut: 요소가 포커스 잃을 때 발생합니다.(버블링O, 오페라만)  
- focus: 요소가 포커스 받을 때 발생합니다.(버블링X)  
- focusin: 요소가 포커스 받을 때 발생합니다.(버블링O, IE 5.5이상, 사파리 5.1이상, 오페라 11.5 이상, 크롬)  
- focusout: 요소가 포커스 잃을 때 발생합니다.(버블링O, IE 5.5이상, 사파리 5.1이상, 오페라 11.5 이상, 크롬)  

이벤트 순서

1. 포커스 잃은 요소에서 focusout 발생  
2. 포커스 받는 요소에서 focusin 발생  
3. 포커스 잃는 요소에서 blur 발생  
4. 포커스 잃는 요소에서 DOMFocusOut 발생  
5. 포커스 받는 요소에서 focus 발생  
6. 포커스 받는 요소에서 DOMFocusIn 발생  
 
지원여부 확인
{% highlight js %}
var isSupported = document.implementation.hasFeature("FocusEvent", "3.0");
{% endhighlight %}

### 마우스 이벤트와 휠 이벤트
DOM레벨 3 정의

- click: 주요 마우스 버튼(왼쪽버튼)이나 엔터키 누를 때 발생(접근성에 구현에 중요함)  
- dblclick: 주요 마우스 버튼 더블클릭시 발생(DOM2에 없음)  
- mousedown: 사용자가 마우스 누를 때 발생  
- mouseenter: 마우스 커서가 요소 경계 안으로 처음 들어올 때 발생(버블링X, DOM2에 없음, IE, 파이어폭스9이상, 오페라)  
- mouseleave: 마우스가 요소 경계 밖으로 나갈 때 발생(버블링X, DOM2에 없음, IE, 파이어폭스9이상, 오페라)  
- mousemove: 마우스가 요소주변을 이동하는 동안 발생  
- mouseout: 커서가 요소 밖으로 나갈때 발생(자식한테 갈 때도 포함)  
- mouseover: 요소 경계 안으로 이동하면 발생  
- mouseup: 버튼을 누르다가 놓을 때 발생  

순서

1. mousedown  
2. mouseup  
3. click  
4. mousedown  
5. mouseup  
6. click  
7. dlbclick  

IE 8 이전 버그로 인한 순서

1. mousedown  
2. mouseup  
3. click  
4. mouseup  
5. dblclick  

{% highlight js %}
// DOM 레벨2 이벤트 확인
var isSupported = document.implementation.hasFeature("MouseEvents", "2.0");
// 위 이벤트 모두 지원 확인
var isSupported = document.implementation.hasFeature("MouseEvent", "3.0");
{% endhighlight %}

마우스 휠에는 mousewheel 이벤트가 하나 존재합니다.

#### 클라이언트 좌표
마우스 이벤트는 clientX, clientY에서 뷰포트 위치를 저장합니다.  

{% highlight js %}
var div = document.getElementById("myDiv");
EventUtil.addHandler(div, "click", function (event) {
	event = Eventutil.getEvent(event);
	console.log("Client coordinates: " + event.clientX + "," + event.clientY);
});
{% endhighlight %}

#### 페이지 좌표
이벤트 객체에 pageX, pageY 프로퍼티에 페이지 좌표가 저장됩니다.  
IE8 이전버전에서는 페이지 좌표를 지원안하지만 클라이언트 좌표와 스크롤정보를 통해 계산할 수 있습니다.  
scrollLeft/scrollTop으로 확인하며, 쿽스모드에서는 document.body, 표준모드에서는 document.documentElement에 저장됩니다.

{% highlight js %}
// 페이지 좌표로 구하기
var div = document.getElementById("myDiv");
EventUtil.addHandler(div, "click", function (event) {
	event = EventUtil.getEvent(event);
	console.log("Page coordinates: " + event.pageX + "," + event.pageY);
});

// IE8 이전에서 스크롤 좌표로 구하기
var div = document.getElementById("myDiv");
EventUtil.addHandler(div, "click", function (event) {
	event = EventUtil.getEvent(event);
	var pageX = event.pageX, pageY = event.pageY;
	
	if (pageX === undefined) {
		pageX = event.clientX + (document.body.scrollLeft || document.documentElement.scrollLeft);
	}
	
	if (pageY === undefined) {
		pageY = event.clientY + (document.body.scrollTop || document.documentElement.scrollTop);
	}
	console.log("Page coordinages: " + pageX + "," + pageY);
});
{% endhighlight %}

#### 화면좌표
전체화면 좌표는 이벤트에 screenX/screenY에 저장됩니다.

{% highlight js %}
var div = document.getElementById("myDiv");
EventUtil.addHandler(div, "click", function (event) {
	event = EventUtil.getEvent(event);
	console.log("Screen coordinates: " + event.screenX + "," + event.screenY);
});
{% endhighlight %}

#### 키보드 수정
키보드의 특정키가 클릭에 영향을 줄 수도 있습니다.  
이러한 수정키들은 Shift, Ctrl, Alt, Meta 키입니다.

{% highlight js %}
var div = document.getElementById("myDiv");
EventUtil.addHandler(div, "click", function (event) {
	event = EventUtil.getEvent(event);
	var keys = new Array();
	
	if (event.shiftKey) {
		keys.push("shift");
	}
	
	if (event.ctrlKey) {
		keys.push("ctrl");
	}
	
	if (event.altKey) {
		keys.push("alt");
	}
	
	if (event.metaKey) {
		keys.push("meta");
	}
	
	console.log("Keys: " + keys.join(","));
});
{% endhighlight %}

#### 관련 요소
mouseover와 mouseout 이벤트에는 목적지요소를 저장하는 관련 요소가 있습니다.  
관련요소는 event객체에 relatedTarget프로퍼티에 있습니다.  
이 프로퍼티는 이 이벤트에서만 값을 가지며 다른 이벤트에서는 null입니다.

{% highlight html %}
<!DOCTYPE html>
<html>
	<head>
		<title>Related Elements Example</title>
	</head>
	<body>
		<div id="myDiv" style="background-color:red;height:100px;width:100px;">
		</div>
	</body>
</html>
{% endhighlight %}

{% highlight js %}
var EventUtil = {
	getRelatedTarget: function (event) {
		if (event.relatedTarget) {
			return event.relatedTarget;
		} else if (event.toElement) {
			return event.toElement;
		} else if (event.fromElement) {
			return event.fromElement;
		} else {
			return null;
		}
	},
	
	// 관계없는 코드 생략됨
};

var div = document.getElementById("myDiv");
EventUtil.addHandler(div, "mouseout", function (event) {
	event = EventUtil.getEvent(event);
	var target = EventUtil.getTarget(event);
	var relatedTarget = EventUtil.getRelatedTarget(event);
	console.log("Moused out of " + target.tagName + " to " + relatedTarget.tagName);
});
{% endhighlight %}

#### 버튼
클릭은 마우스 기본 버튼이나 엔터키를 눌렀을 때만 발생합니다.  
mousedown과 mouseup이벤트는 event객체에 어떤 버튼을 눌렀는지 알려주는 button프로퍼티가 있습니다.  

표준
0: 기본 버튼(보통 왼쪽)
1: 마우스 가운데버튼(휠)
2: 두번째 마우스 버튼(보통 오른쪽)

IE8이전  
0: 아무 버튼도 안누름  
1: 기본 버튼 누름  
2: 두번째 마우스 버튼 누름  
3: 기본 버튼과 두번째 버튼 누름  
4: 가운데 버튼 누름  
5: 기본 버튼과 가운데 버튼 누름  
6: 두번째 버튼과 가운데 버튼 누름  
7: 세버튼을 모두 누름  

크로스 브라우징 코드를 만들때는 보통 DOM방식으로 통일합니다.  
익스플로러의 5나 7을 반환하는 이벤트는 DOM에서는 0입니다.  

{% highlight js %}
var EventUtil = {
	getButton: function (event) {
		if (document.implementation.hasFeature("MouseEvents", "2.0")) {
			return event.button;
		} else {
			switch (event.button) {
			case 0:
			case 1:
			case 3:
			case 5:
			case 7:
				return 0;
			case 2:
			case 6:
				return 2;
			case 4:
				return 1;
			}
		}
	}
	
	// 이미 설명한 코드는 생략
};

var div = document.getElementById("myDiv");
EventUtil.addHandler(div, "mousedown", function (event) {
	event = EventUtil.getEvent(event);
	console.log(EventUtil.getButton(event));
});
{% endhighlight %}

#### 추가적인 이벤트 정보
DOM레벨2에서는 이벤트 객체에 추가 정보를 제공하는 detail프로퍼티가 있습니다.  
detail값은 얼마나 많이 클릭했는지를 보여주며 mousedown과 mouseup사이에 움직이면 0으로 초기화 됩니다.  

IE의 이벤트 추가정보

- altLeft: 왼쪽 Alt키 눌림  
- ctrlLeft: 왼쪽 Ctrl키 눌림  
- offsetX: 타깃 경계기준 x 좌표  
- offsetY: 타깃 경계기준 y 좌표  
- shiftLeft: 왼쪽 shift키 눌렸는지 알려줌

#### mousewheel 이벤트
이 이벤트는 모든 요소에서 발생하며 document(IE8)와 window(IE9 이상, 오페라, 크롬, 사파리)까지 버블링해 올라갑니다.  
wheelDelta 프로퍼티  
일반적  
휠 앞으로 굴림: +120  
휠 뒤로 굴림: -120  
오페라 9.5미만: 부호 반대  

DOMMouseScroll 이벤트: 파이어폭스의 마우스 휠 이벤트  
detail 프로퍼티  
휠 앞으로 굴림: -3  
휠 뒤로 굴림: +3  

존재하지 않는 이벤트에 이벤트 핸들러를 할당하면 에러없이 실패합니다.

{% highlight js %}
// 일반적 마우스 휠 이벤트
EventUtil.addHandler(document, "mousewheel", function (event) {
	event = EventUtil.getEvent(event);
	console.log(event.wheelDelta);
});

// 오페라 마우스 휠 이벤트
EventUtil.addHandler(document, "mousewheel", function (event) {
	event = EventUtil.getEvent(event);
	var delta = (client.engine.opera && client.engine.opera < 9.5 ? 
		-event.wheelDelta : event.wheelDelta);
		console.log(delta);
});

// 파이어폭스 마우스 휠 이벤트
EventUtil.addHandler(document, "DOMMouseScroll", function (event) {
	event = EventUtil.getEvent(event);
	console.log(event.detail);
});

// 크로스 브라우저 솔루션
var EventUtil = {
	getWheelDelta: function (event) {
		if (event.wheelDelta) {
			return (client.engine.opera && client.engine.opera < 9.5 ?
				-event.wheelDelta : event.wheelDelta);
		} else {
			return -event.detail * 40;
		}
	},
	
	// 이미 설명한 코드는 생략
};

// 크로스 브라우저 솔루션 이벤트핸들러 등록
(function() {
	event = EventUtil.getEvent(event);
	var delta = EventUtil.getWheelDelta(event);
	console.log(delta);
	
	EventUtil.addHandler(document, "mousewheel", handleMouseWheel);
	EventUtil.addHandler(document, "DOMMouseScroll", handleMouseWheel);
})();
{% endhighlight %}

#### 터치장치 지원
터치장치 지원할 때 고려할 사항들

- dblclick 이벤트는 전혀 지원되지 않습니다.  
- 클릭 가능한 요소를 탭하면 mousemove이벤트가 발생합니다.  
- mousemove 이벤트도 mouseover와 mouseout 이벤트를 발생시킵니다.  
- mousewheel과 scroll 이벤트는 화면에 두 손가락을 올릴 때 발생하며 움직이면 스크롤됩니다.  

#### 접근성 문제
스크린 리더 사용자가 쓰기 쉽게 하려면 다음을 주의해야 합니다.

- 코드 실행에는 click을 이용합니다.  
- 사용자에게 새 option을 제시할 때는 스크린리더 사용자를 고려해야합니다.  
- 중요한 동작을 dblclick으로 실행시키면 안됩니다.  

### 키보드와 텍스트 이벤트
DOM레벨2에서는 키보드 이벤트를 정의했다가 제거됬습니다.
그래서 대부분의 브라우저는 DOM레벨0에 영향을 받고, DOM레벨3 명세를 따르는 브라우저도 있습니다.

- keydown: 사용자가 키를 처음 누를 때 발생  
- keypress: 사용자가 키를 누르고 문자가 입력되었을 때 처음 입력되며 누르고 있는 동안 계속 발생  
- keyup: 사용자가 키에서 손을 뗄 때 발생  

keydown과 keypress모두 텍스트 박스에 글자가 생기기 전에 발생하지만 keyup은 글자가 생기고 발생합니다.  

#### 키 코드
keydown과 keyup이벤트에서는 keyCode프로퍼티에 각 키에 대응하는 코드가 있습니다.

{% highlight js %}
var textbox = document.getElementById("myText");
EventUtil.addHandler(textbox, "keyup", function (event) {
	event = EventUtil.getEvent(event);
	console.log(event.keyCode);
});
{% endhighlight %}

#### 문자코드
keypress 이벤트객체에는 charCode라는 문자의 아스키코드를 저장하는 프로퍼티를 지원합니다.  

{% highlight js %}
// 크로스 브라우저 코드
var EventUtil = {
	getCharCode: function (event) {
		if (typeof event.charCode == "number") {
			return event.charCode;
		} else {
			return event.keyCode;
		}
	},
	
	// 이미 구현한 코드는 설명안함
};

// 적용
var textbox = document.getElementById("myText");
EventUtil.addHandler(textbox, "keypress", function (event) {
	event = EventUtil.getEvent(event);
	console.log(EventUtil.getCharCode(event);
};
{% endhighlight %}

#### DOM 레벨 3에서 바뀐점
- charCode 폐기 -> key, char 프로퍼티 추가  
- key: 문자면 문자 반환, 그 외에는 키 이름 반환  
- char: 문자면 문자 반환, 그 외는 null 반환  
- IE9: key지원 char미지원  
- 사파리5, 크롬: key에서 문자아니면 유니코드 반환  
- 크로스브라우저 구현 불가 -> key, keyIdentifier, char 안쓰길 권장  

- location: 어디에서 눌렸는지 알려줌(사용 않길 권장)  
  - keyLocation: 사파리, 크롬에서 location 구현(버그: 3만 동작)  
	1: 기본 키보드  
	2: 왼쪽(왼쪽 알트키)  
	3: 키패드  
	4: 모바일 환경  
	5: 조이스틱  

- getModifierState: 수정키 눌려있는지 확인가능(IE만 구현)
  - 나머지 브라우저는 shiftKey, altKey, ctrlKey metaKey에서 사용가능  

- textInput 이벤트: 문자가 눌렸을 때만 작동함  
  - event객체에 inputMethod라는 입력방법을 알려주는 프로퍼티 존재  
    0: 브라우저 입력 모름  
	1: 키보드  
	2: 텍스트 붙여넣기  
	3: 드래그 앤 드롭  
	4: IME 통한 입력  
	5: 폼 옵션으로 입력  
	6: 스타일러스 등으로 입력  
	7: 목소리로 입력  
	8: 여러방법 조합  
	9: 스크립트로 입력

#### 장치별 키보드 이벤트
Wii리모트에 저장
	
#### 조합 이벤트
DOM 레벨3에서 처음 도입되었으며 IME의 입력을 처리하도록 만들어졌습니다.  
현재 조합이벤트를 지원하는 브라우저는 IE9 이상밖에 없습니다.

조합이벤트  

- compositionstart: IME 텍스트 조합 열리는 순간 발생(data property는 입력중인 문자입니다(선택만))  
- compositionupdate: 새문자가 삽입될 때 발생(삽입될 새 문자입니다.)  
- compositionend: 시스템이 닫힐 때 발생(IME 세션에서 입력된 문자입니다.)

{% highlight js %}
var textbox = document.getElementById("myText");
EventUtil.addHandler(textbox, "compositionstart", function (event) {
	event = EventUtil.getEvent(event);
	console.log(event.data);
});

EventUtil.addHandler(textbox, "compositionupdate", function (event) {
	event = EventUtil.getEvent(event);
	console.log(event.data);
});

EventUtil.addHandler(textbox, "compositionend", function (event) {
	event = EventUtil.getEvent(event);
	console.log(event.data);
});

// 지원여부 확인
var isSupported = document.implementation.hasFeature("CompositionEvent", "3.0");
{% endhighlight %}

### metation event
DOM 레벨2에서는 DOM일부가 변경되었을 때 이벤트로 알려줍니다.  

- DOMSubtreeModified: DOM구조가 바뀌었을 때 발생합니다.(다른 이벤트가 모두 발생하고 발생)  
- DOMNodeInserted: 노드가 다른 노드의 자식으로 삽입될 때 발생합니다.  
- DOMNodeRemoved: 노드가 부모 노드로 부터 제거될 때 발생합니다.  
- DOMNodeInsertedIntoDocument: 노드가 직접적으로 또는 자신이 존재하는 서브트리를 통해 삽입되었을 때 발생합니다.(DOM레벨3에서 폐기)  
- DOMNodeRemovedFromDocument: 노드가 직접적으로 또는 자신이 존재하는 서브트리에서 제거되었을 때 발생합니다.(DOM 레벨3에서 폐기)  
- DOMAttrModified: 속성이 바뀌었을 때 발생(DOM레벨3에서 폐기)  
- DOMCharacterDataModified: 텍스트 노드의 값이 바뀔 때 발생(DOM 레벨3에서 폐기)  

지원여부확인
{% highlight js %}
var isSupported = document.implementation.hasFeature("MutationEvents", "2.0");
{% endhighlight %}

#### 노드 제거
1. removeChild나 replaceChild  
2. DOMNodeRemoved 이벤트 발생(버블링됨, target: 제거된 노드, event.relatedNote: 부모 노드 참조)  
3. 자식노드 존재시에 폐기된 DOMNodeRemovedFromDocument 이벤트가 각 자식노드에서 발생한 다음 제거된 노드에서 발생  
4. 제거  
5. DOMSubtreeModified 이벤트 발생(target: 제거된 노드의 부모노드)  

{% highlight html %}
<!DOCTYPE html>
<html>
	<head>
		<title>Node Removal Events Example</title>
	</head>
	<body>
		<ul id="myList">
			<li>Item 1</li>
			<li>Item 2</li>
			<li>Item 3</li>
		</ul>
	</body>
</html>
{% endhighlight %}
테스트 코드
{% highlight js %}
EventUtil.addHandler(window, "load", function (event) {
	var list = document.getElementById("myList");
	
	EventUtil.addHandler(document, "DOMSubtreeModified", function (event) {
		console.log(event.type);
		console.log(event.target);
	});
	
	EventUtil.addHandler(document, "DOMNodeRemoved", function (event) {
		console.log(event.type);
		console.log(event.target);
		console.log(event.relatedNode);
	});
	
	EventUtil.addHandler(list.firstChild, "DOMNodeRemovedFromDocument", function (event) {
		console.log(event.type);
		console.log(event.target);
	});
	
	list.parentNode.removeChild(list);
});
{% endhighlight %}

#### 노드 삽입
1. appendChild나 replaceChild, insertBefore 호출  
2. DOMNodeInserted(버블링됨, target: 삽입된 노드, event.relatedNode: 부모노드에 대한 참조)  
3. 폐기된 DOMNodeInsertedIntoDocument 이벤트 발생(버블링 안됨, target: 삽입된 노드, event: 정보없음)  
4. 삽입  
5. DOMSubtreeModified 이벤트 발생  

실행 코드  
{% highlight js %}
EventUtil.addHandler(window, "load", function (event) {
	var list = document.getElementById("myList");
	var item = document.createElement("li");
	item.appendChild(document.createTextNode("Item 4"));
	
	EventUtil.addHandler(document, "DOMSubtreeModified", function (event) {
		console.log(event.type);
		console.log(event.target);
	});
	
	EventUtil.addHandler(document, "DOMNodeInserted", function (event) {
		console.log(event.type);
		console.log(event.target);
		console.log(event.relatedNode);
	});
	
	EventUtil.addHandler(list.firstChild, "DOMNodeInsertedIntoDocument", function (event) {
		console.log(event.type);
		console.log(event.target);
	});
	
	list.appendChild(list);
});
{% endhighlight %}

### HTML5 이벤트

#### contextmenu 이벤트
컨텍스트 메뉴가 생기려는 순간에 발생합니다.  

- 버블링됨  
- target: 해당요소  
- 이벤트 막기: DOM표준준수(event.preventDefault), IE8이전(event.returnValue)  
- 브라우저 기본동작 막아야함  
- 지원: IE, 파이어폭스, 사파리, 크롬, 오페라11이상  

{% highlight html %}
<html>
	<body>
		<div id="myDiv">
			Right click or Ctrl+click me to get a custom context menu.Click  
			anywhere else to get the default context menu.
		</div>
		<ul id="myMenu" style="position:absolute;visibility:hidden;background-color:silver">
			<li><a href="http://www.nczonline.net">Nicholas' site</a></li>
			<li><a href="http://www.wrox.com">Wrox site</a></li>
			<li><a href="http://www.yahoo.com">Yahoo!</a></li>
		</ul>
	</body>
</html>
{% endhighlight %}

{% highlight js %}
EventUtil.addHandler(window, "load", function (event) {
	var div = document.getElementById("myDiv");
	
	EventUtil.addHandler(document, "contextmenu", function (event) {
		event = EventUtil.getEvent(event);
		EventUtil.preventDefault(event);
		
		var menu = document.getElementById("myMenu");
		menu.style.left = event.clientX + "px";
		menu.style.top = event.clientY + "px";
		menu.style.visibility = "visible";
	});
	
	EventUtil.addHandler(document, "click" function (event) {
		document.getElementById("myMenu").style.visibility = "hidden";
	});
});
{% endhighlight %}

#### beforeunlod 이벤트
window에서 발생하며 페이지 떠나는 것을 막기위해 만들어졌습니다.  
IE, 파이어폭스, 사파리, 크롬, 오페라(12이상)이 지원합니다.  

{% highlight js %}
EventUtil.addHandler(window, "beforeunload", function (event) {
	event = EventUtil.getEvent(event);
	var message = "I'm really going to miss you if you go.";
	event.returnValue = message;
	return message;
});
{% endhighlight %}


#### DOMContentLoaded 이벤트
DOM트리가 완전히 구성되는 즉시 발생(자바스크립트, CSS등 파일 안기다림)

{% highlight js %}
EventUtil.addHandler(document, "DOMContentLoaded", function (event) {
	console.log("Content loaded");
});

// 지원 안하는 경우 밀리초 타임아웃 사용
setTimeout(function () {
	// 이벤트 핸들러 등록
}, 0);
{% endhighlight %}

#### readystatechange 이벤트
문서나 요소를 불러오는 상황에 대한 정보를 제공  

readyState 프로퍼티(같은 순서로 진행되지는 않음)  
interactive인 시간은 DOMContentLoaded 이벤트가 발생하는 시간과 비슷합니다.(complete보다 빨리 일어날 때도 있음)  
script나 link요소에서도 발생하므로 외부 자바스크립트나 CSS를 불러왔는지 판단 가능합니다.  
  

- uninitialized: 객체가 존재하지만 초기화 안됬습니다.  
- loading: 객체에서 데이터를 불러오는 중입니다.  
- loaded: 객체에서 데이터를 완전히 불러왔습니다.  
- interactive: 객체를 완전히 불러오지는 못했지만 상호작용 가능합니다.  
- complete: 객체를 완전히 불러왔습니다.  

{% highlight js %}
// 사용 예
EventUtil.addHandler(document, "readystatechange", function (event) {
	if (document.readyState == "interactive") {
		console.log("Content loaded");
	}
});

// complete와 interactive 중 빠른 것 먼저실행
EventUtil.addHandler(document, "readystatechange", function (event) {
	if (document.readyState == "interactive" ||
		document.readyState == "complete") {
		EventUtil.removeHandler(document, "readystatechange", arguments.callee);
		console.log("Content loaded");
	}			
});

// 외부 자바스크립트 로드 예제
EventUtil.addHandler(window, "load", function () {
	var script = document.createElement("script");
	
	EventUtil.addHandler(script, "readystatechange", function (event) {
		event = EventUtil.getEvent(event);
		var taget = EventUtil.getTarget(event);
		
		if (target.readyState == "loaded" || target.readyState == "complete") {
			EventUtil.removeHandler(target, "readystatechange", arguments.callee;
			console.log("Script Loaded");
		}
	});
	script.src = "example.js";
	document.body.appendChild(script);
});

// 외부 CSS 로드 예제
// 외부 CSS 로드 예제
EventUtil.addHandler(window, "load", function () {
	var script = document.createElement("link");
	link.type = "text/css";
	link.rel = "stylesheet";
	
	EventUtil.addHandler(link, "readystatechange", function (event) {
		event = EventUtil.getEvent(event);
		var taget = EventUtil.getTarget(event);
		
		if (target.readyState == "loaded" || target.readyState == "complete") {
			EventUtil.removeHandler(target, "readystatechange", arguments.callee;
			console.log("CSS Loaded");
		}
	});
	link.src = "example.css";
	document.getElementsByTagName("head")[0].appendChild(link);
});
{% endhighlight %}

#### pageshow, pagehide 이벤트  
파이어폭스와 오페라는 bfcache라는 기능을 도입했습니다.(캐시)  

pageshow 이벤트

- load보다 늦게 발생하며 페이지 상태를 복원하는 즉시 발생합니다.  
- document에서 발생하지만 window에 이벤트 핸들러 등록  
- 이벤트 객체에 persisted프로퍼티: bfcache에 저장된 경우 true반환  

pagehide 이벤트  

- unload이벤트 전에 발생  
- document에서 발생하지만 window에 이벤트 핸들러 등록  
- 이벤트 객체에 persisted프로퍼티: 페이지 떠난 다음 bfcache에 저장될 때 true

두 이벤트 지원: 파이어폭스, 사파리 5이상, 크롬, 오페라, IE9

{% highlight js %}
// pageshow 테스트
(function() {
	var showCount = 0;
	
	EventUtil.addHandler(window, "load", function () {
		console.log("Load fired");
	});
	
	EventUtil.addHandler(window, "pageshow", function () {
		showCount++;
		console.log("Show has been fired " + showCount + " times.");
	});
})();

// pagehide 이벤트
(function() {
	var showCount = 0;
	
	EventUtil.addHandler(window, "load", function () {
		console.log("Load fired");
	});
	
	EventUtil.addHandler(window, "pageshow", function () {
		showCount++;
		console.log("Show has fired " + showCount + " times. Persisted? " + event.persisted);
	});
})();
{% endhighlight %}

#### hashchange 이벤트
url해시가 바뀔 때 발생합니다.  
이벤트 객체에는 oldURL과 newURL 두가지가 추가됩니다.  
지원: IE8이상, 파이어폭스 3.6이상, 크롬, 오페라 10.6이상

{% highlight js %}
EventUtil.addHandler(window, "hashchange", function (event) {
	console.log("Old URL: " + event.oldURL + "\nNew URL: " + event.newURL);
});

EventUtil.addHandler(window, "hashchange", function (event) {
	console.log("Current hash: " + location.hash);
});

// 지원 여부 확인
var isSupported = ("onhashchange" in window);

// IE8에서 IE7문서모드일 때 지원안하는데 지원한다고 버그있음
// 해결소스
var isSupported = ("onhashchange" in window) &&
				(document.documentMode === undefined ||
					document.documentMode > 7);
{% endhighlight %}
## 참고자료  
Nicholas C. Zakas. (2013). 프론트엔드 개발자를 위한 자바스크립트 프로그래밍, (한선용 옮김). 인사이트  
[MDN event](https://developer.mozilla.org/en-US/docs/Web/API/Event)  