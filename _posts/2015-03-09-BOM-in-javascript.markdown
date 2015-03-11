---
layout: article
title:  "브라우저 객체 모델"
date:   2015-03-09 16:31:13
image:
  teaser: javascript_book.jpg
categories: book
---

# 브라우저 객체 모델 정리
BOM은 웹페이지 콘텐츠와 무관하게 브라우저 기능을 노출하는 객체입니다.  

## window 객체
브라우저 창의 자바스크립트 인터페이스 구실을 하고, ECMA Script Global객체로 기능합니다.  

### 전역 스코프
전역에서 선언한 변수와 함수는 모두 window 객체의 프로퍼티 및 메서드가 됩니다.  
{% highlight js %}
var age 29;
function sayAge() {
  console.log(this.age);
}

console.log(window.age); // 29
sayAge();          // 29
window.sayAge();   // 29
{% endhighlight %}
전역변수와 window프로퍼티로 정의한 것의 차이는 window프로퍼티만 delete로 삭제할 수 있다는 것입니다.  
또한 선언한적 없는 변수에 접근하면 에러가 나지만 선언하지 않았을 가능성이 있는 변수는 window객체를 통해 확인가능합니다.  
{% highlight js %}
var age = 29;
window.color = "red";

delete window.age;    // IE9 미만에서는 에러, 나머지는 false반환
delete window.color;  // IE9 미만에서는 에러, 나머지는 true반환
console.log(window.color);  // undefined

var newValue = oldValue;          // oldValue를 선언한 적이 없어서 에러가 발생  
var new Value = window.oldValue;  // 프로퍼티 검색이므로 에러가 발생하지 않음  
{% endhighlight %}
var 연산자를 써서 window에 추가한 프로퍼티는 [[Configurable]] 속성이 false로 지정되있어서 delete 연산자를 사용할 수 없습니다.  

### 창 사이의 관계와 프레임
페이지에 프레임이 있으면 각 프레임은 독자적인 window객체를 가지고 이 객체들은 frames 컬렉션에 저장됩니다.  
frames 컬렉션에서 window객체 등은 인덱스(왼쪽에서 오른쪽, 위에서 아래방향)와 프레임 이름으로 색인합니다.  
각 window 객체는 프레임의 name을 프로퍼티로 가집니다.  

###### top객체
top객체는 항상 최상위 프레임인 브라우저창을 가리킵니다.  

###### parent객체
parent객체는 현재 프레임의 상위 부모 프레임을 가리킵니다.  

###### self 객체
self객체는 자기 자신을 가리킵니다.  

이 객체들은 window의 프로퍼티여서 window.parent, window.top도 가능합니다.  
프레임을 사용하면 브라우저에 Global 객체가 여러개 존재하게 됩니다.  
사용예  
{% highlight js %}
// 가장 최상위 프레임 호출
top.frames[0];
window.frames[0];
frames[0];

// 바로 위 프레임 호출
parent;

// 자기 자신 프레임
self;

// window 객체 이용한 체인
window.parent.parent.frames[0];
{% endhighlight %}

### 창의 위치
창의 왼쪽위를 기준으로 얼마나 떨어져 있는지 알려주는 함수  
screenLeft/screenTop: IE, 사파리, 오페라, 크롬  
screenX/screenY: 파이어폭스, 사파리, 크롬  
(주의, 오페라의 screenX, screenY는 다르게 동작하므로 사용하면 안됨)  
{% highlight js %}
// 크로스 브라우저 코드
var leftPos = (typeof window.screenLeft == "number") ? window.screenLeft : window.screenX;
var topPos = (typeof window.screenTop == "number") ? window.screenTop : window.screenY;
{% endhighlight %}
브라우저 창의 좌표를 브라우저에 무관하게 일관적으로 구할 방법은 없습니다.  

창을 정확한 위치로 이동하는 함수  
moveTo: x/y 좌표로 이동  
moveBy: 각 방향으로 이동할 거리  
브라우저에서는 이 메서드를 금지할 가능성이 잇으며 IE7이상, 크롬, 오페라에서는 기본적으로 비활성화 되어있습니다.  
이 메서드들은 오직 최상위 창에서만 동작합니다.  

### 창 크기
창의 크기를 반환하는 함수  
innerWidth/innerHeight: IE9이상, 사파리, 파이어폭스, 오페라, 크롬 - 창 내부의 페이지 뷰포트 크기 반환(테두리, 툴바 제외)  
outerWidth/outerHeight: IE9이상, 사파리, 파이어폭스, 크롬 - 항상 브라우저 창의 크기 반환  
                        오페라 - 페이지 뷰포트 크기  

document.documentElement.clientWidth/document.documentElement.clientHeight: IE(6은 표준만), 파이어폭스, 사파리, 오페라, 크롬 - 페이지 뷰포트의 너비와 높이  
document.body.clientWidth/document.body.clientHeight: IE6 쿽스모드에서 위와 같은 동작함  
document.documentElement/document.body의 clientWidth/clientHeight: 크롭 - 페이지 뷰포트 크기  

{% highlight js %}
// 크로스 브라우저 코드
var pageWidth = window.innerWidth, pageHeight = window.innerHeight;

if (typeof pageWidth != "number") {
  if (document.compatMode == "CSS1Compat") {
    pageWidth = document.documentElement.clientWidth;
    pageHeight = document.documentElement.clientHeight;
  } else {
    pageWidth = document.body.clientWidth;
    pageHeight = document.body.clientHeight;
  }
}
{% endhighlight %}
resizeTo - x/y 크기로 변환  
resizeBy - x/y만큼 변환  

### 내비게이션과 열기  
window.open 함수  
1번째 매개변수: 이동할 URL  
2번째 매개변수: target, 대상 창 - 이 값에 해당하는 창/프레임이 있으면 해당 창/프레임에서 URL을 엽니다.  
                              - 이 값에 해당하는 창/프레임이 없으면 이 값을 이름으로 한 새 창을 만듭니다.  
                              - _self,_parent,_top,_blank 같은 속성 사용가능  
3번째 매개변수: 기능을 나타내는 문자열 - 새 창을 띄울 때의 옵션값을 설정합니다. 이 값은 쉼표로 구분됩니다.  

#### 팝업 창

<table>
  <tr>
    <th>Option</th>
    <th>Values</th>
    <th>Description</th>
    <th>Default</th>
  </tr>
  <tr>
    <td>location</td>
    <td>yes|no</td>
    <td>Does the location bar show?</td>
    <td></td>
  </tr>
  <tr>
    <td>menubar</td>
    <td>yes|no</td>
    <td>Does the menubar show?</td>
    <td>no</td>
  </tr>
  <tr>
    <td>scrollbars</td>
    <td>yes|no</td>
    <td>Do scrollbars show?</td>
    <td>no</td>
  </tr>
  <tr>
    <td>status</td>
    <td>yes|no</td>
    <td>Does the status bar show|</td>
    <td></td>
  </tr>
  <tr>
    <td>titlebar</td>
    <td>yes|no</td>
    <td>Does the titlebar show?</td>
    <td></td>
  </tr>
  <tr>
    <td>toolbar</td>
    <td>yes|no</td>
    <td>Does the toolbar show?</td>
    <td>no</td>
  </tr>
  <tr>
    <td>resizable</td>
    <td>yes|no</td>
    <td>Can you resize the window?</td>
    <td>no</td>
  </tr>
  <tr>
    <td>height</td>
    <td>pixels</td>
    <td>height of window</td>
    <td>Min 100</td>
  </tr>
  <tr>
    <td>width</td>
    <td>pixels</td>
    <td>width of window</td>
    <td>Min 100</td>
  </tr>
  <tr>
    <td>directories</td>
    <td>yes|no</td>
    <td>Does the personal toolbar show?</td>
    <td></td>
  </tr>
  <tr>
    <td>innerHeight</td>
    <td>pixels</td>
    <td>specifies the inner height of window</td>
    <td></td>
  </tr>
  <tr>
    <td>innerWidth</td>
    <td>pixels</td>
    <td>specifies the inner width of window</td>
    <td></td>
  </tr>
  <tr>
    <td>screenX</td>
    <td>pixels</td>
    <td>specifies distance from left edge of screen</td>
    <td></td>
  </tr>
  <tr>
    <td>screenY</td>
    <td>pixels</td>
    <td>specifies distance from top edge of screen</td>
    <td></td>
  </tr>
</table>

{% highlight js %}
var wroxWin = window.open("http://www.wrox.com/","wroxWindow", "height=400,width=400,top=10,left=10,resizable=yes");

// 크기조절
wroxWin.resizeTo(500,500);

// 이동
wroxWin.moveTo(100,100);

// 닫기
wroxWin.close(); // window.open으로 생성한 팝업창에서만 동작
console.log(wroxWin.closed); // true

console.log(wroxWin.opener == window); // true
{% endhighlight %}
opener프로퍼티: 최상위 window의 객체로 자신을 연 창을 참조함 - null로 변경시 프로세스를 분리할 수 있음  

#### 보안 제한

- 팝업창을 화면밖에 생성하거나 이동불가, 상태바 끌기 불가(IE6)  
- 팝업 창에서 주소표시줄 끌기 불가, 이동, 크기 조절 금지(IE7)  
- 팝업창의 상태바 감출 수 없음(파이어폭스1)  
- 주소표시줄 숨길수 없음(파이어폭스3)  
- 팝업 창을 주요 브라우저 안에서만 열 수 있으며, 시스템 대화상자와 혼동되는 위치에 불가(오페라)  
- 사용자 행동없는 상태에서 팝업창 호출하면 팝업창 제목만 노출(크롬)  

#### 팝업 차단
팝업차단기를 사용하면 예기치 못한 팝업을 차단할 수 있습니다.  
팝업차단기가 작동되면 window.open()은 null을 반환합니다.  
하지만 외부 프로그램이 팝업을 차단한 경우에는 에러를 반환합니다.  

{% highlight js %}
var blocked = false;

try {
  var wroxWin = window.open("http://www.wrox.com", "_blank");
  if (wroxWin = null) {
    blocked = true;
  }
} catch (ex) {
  blocked = true;
}

if(blocked) {
  console.log("The popup was blocked!");
}
{% endhighlight %}

### 인터벌과 타임아웃
인터벌: 일정시간 동안 반복해서 실행 - setInterval()  
타임아웃: 일정시간 후에 실행 - setTimeout()  

#### 타임아웃
{% highlight js %}
setTimeout("console.log('Hello world!')", 1000);

var timeoutId = setTimeout(function() {
  console.log("Hello world!");
  }, 1000);

clearTimeout(timeoutId);
{% endhighlight %}
첫번째 매개변수에 eval처럼 작동하게 할 수 있지만 성능이 좋지 않습니다.  
두번째 매개변수인 대기시간은 정확한 시간이 아닌 그 후에 작업 큐에 들어갑니다.  
숫자형 ID를 반환하여 취소할 수 있습니다.  
timeout의 this 값은 스트릭트 모드가 아니면 항상 window입니다.  

#### 인터벌
매개변수와 리턴값은 timeout과 같습니다.  
clearInterval로 취소할 수 있습니다.  

인터벌을 대신해서 아래와 같이 timeout을 사용할 수 있습니다.(권장)  
{% highlight js %}
var num = 0;
var max = 10;

function incrementNumber() {
  num++;

  if (num < max) {
    setTimeout(incrementNumber, 500);
  } else {
    console.log("Done");
  }
}

setTimeout(incrementNumber, 500);
{% endhighlight %}

### 시스템 대화상자  
console.log - 알림  
confirm - 확인 취소 (boolean 반환)  
prompt - 입력받는 대화상자 (대화상자 표시할 텍스트, 기본값을 매개변수로 가지며 입력받은 값을 반환합니다.)  
find - 비동기적인 찾기 대화상자  
print - 프린트 대화상자  

외관은 운영체제/브라우저 설저에 따릅니다.  
크롬 오페라는 첫번째 대화상자 다음부터는 사용자가 대화상자를 차단할 수 있게 체크박스를 표시해줍니다.  

## location 객체  
현재 창에 불러온 문서 정보와 함께 일반적인 내비게이션기능을 제공  
window의 프로퍼티인 동시에 document의 프로퍼티(같은 객체)  

<table>
  <tr>
    <th style="width:20%">Property</th>
    <th>Description</th>
    <th>Example</th>
  </tr>
  <tr>
    <td>hash</td>
    <td>Sets or returns the anchor part (#) of a URL</td>
    <td>#contents</td>
  </tr>
  <tr>
    <td>host</td>
    <td>Sets or returns the hostname and port number of a URL</td>
    <td>www.wrox.com:80</td>
  </tr>
  <tr>
    <td>hostname</td>
    <td>Sets or returns the hostname of a URL</td>
    <td>www.wrox.com</td>
  </tr>
  <tr>
    <td>href</td>
    <td>Sets or returns the entire URL</td>
    <td>http://www.wrox.com</td>
  </tr>
  <tr>
    <td>origin</td>
    <td>Returns the protocol, hostname and port number of a URL</td>
    <td></td>
  </tr>
  <tr>
    <td>pathname</td>
    <td>Sets or returns the path name of a URL</td>
    <td>/WileyCDA</td>
  </tr>
  <tr>
    <td>port</td>
    <td>Sets or returns the port number of a URL</td>
    <td>8080</td>
  </tr>
  <tr>
    <td>protocol</td>
    <td>Sets or returns the protocol of a URL</td>
    <td>http:</td>
  </tr>
  <tr>
    <td>search</td>
    <td>Sets or returns the querystring part of a URL</td>
    <td>?q=javascript</td>
  </tr>
</table>  

### 쿼리스트링 확장  
쿼리스트링 파싱하는 코드  
{% highlight js %}
var qs = (location.search.length > 0 ? location.search.substring(1) : ""),
    args = {},
    items = qs.length ? qs.split("&") : [],
    item = null,
    name = null,
    value = null,
    i = 0,
    len = items.length;

    for (i=0; i < len; i++) {
      item = items[i].split("=");
      name = decodeURIComponent(item[0]);
      value = decodeURIComponent(item[1]);

      if(name.length) {
        args[name] = value;
      }
    }
    return args;
}
{% endhighlight %}

### location 조작  
assign, href - 새 URL 이동 및 브라우저 히스토리 스택에 추가  
location 객체 프로퍼티 변경 - 현재 페이지 영향줌  
replace - 이동하지만 히스토리 스택에 기록이 남지는 않음  
reload - 페이지 갱신 가능하면 캐시에서 읽어옴(실행여부를 확실히 알수 없음 - 코드 끝에 두는것을 권장)  

{% highlight js %}
location.assign("http://www.wrox.com");

window.location = "http://www.wrox.com";
location.href = "http://www.wrox.com";

// 현재 http://www.wrox.com/WileyCDA/에 있다고 가정

// http://www.wrox.com/WIleyCDA/#section1으로 변경
location.hash = "#section1";

// http://www.wrox.com/WileyCDA/?q=javascript로 변경
location.search = "?q=javascript";

// http://www.yahoo.com/WileyCDA/로 변경
location.hostname = "www.yahoo.com";

// http://www.yahoo.com/mydir/로 변경
location.pathname = "mydir";

location.replace("http://www.wrox.com/");

location.reload();     // 가능하면 캐시에서
location.reload(true); // 항상 서버에서
{% endhighlight %}

## navigator 객체
넷스케이프 네비게이터 2에서 도입했고, 클라이언트에서 브라우저 구별 밥법 표준으로 쓰였습니다.  
참고: [MDN navigator api](https://developer.mozilla.org/ko/docs/Web/API/Navigator)  

### 플러그인 감지
plugins 배열에서 정보를 얻을 수 있습니다.  

- name: 플러그인 이름  
- description: 플러그인 설명  
- filename: 플러그인의 파일 이름  
- length: 플러그인이 처리하는 마임타입 숫자  

플러그인은 루프를 돌면서 원하는 이름과 name을 대조하는 식입니다.  
이 방법은 파이어폭스, 사파리, 오페라, 크롬에서 동작합니다.  
{% highlight js %}
function hasPlugin(name) {
  name = name.toLowerCase();
  for(var i = 0; i < navigator.plugins.length; i++) {
    if (navigator.plugins[i].name.toLowerCase().indexOf(name) > -1) {
      return true;
    }
  }
  return false;
}

console.log(hasPlugin("Flash"));
console.log(hasPlugin("QuickTime"));
{% endhighlight %}

인터넷 익스플로러는 플러그인 탐지가 어렵습니다.  
플러그인을 탐지하는 유일한 방법은 ActiveXObject를 사용하는 것입니다.  
인터넷 익스플로러는 COM객체로 플러그인을 구현해서 해당 COM 식별자를 알아야 확인 가능합니다.  
{% highlight js %}
function hasPlugin(name) {
  try {
    new ActiveXObject(name);
    return true;
  } catch (ex) {
    return false;
  }
}

console.log(hasIEPlugin("ShockwaveFlash.ShockwaveFlash"));
console.log(hasIEPlugin("QuickTime.QuickTime"));
{% endhighlight %}

### 처리기 등록  
registerContentHandler() - 처리할 마임타입, 해당 마임타입 처리할 페이지URL, 애플리케이션 이름  
registerProtocolHandler() - 처리할 프로토콜, 해당 프로토콜 처리할 페이지URL, 애플리케이션 이름  
{% highlight js %}
navigator.registerContentHandler("application/rss+xml", "http://www.somereader.com?feed=%s", "Some Reader");
navigator.registerProtocolHandler("mailto", "http://www.somemailclient.com?cmd=%s", "Some Mail Client");
{% endhighlight %}

## screen 객체  
클라이언트 화면에 관련된 객체  
브라우저에서 기능을 제한할 때가 많으므로 동작하지 않을 때가 많습니다.  
참고: [MDN screen api](https://developer.mozilla.org/en-US/docs/Web/API/Window/screen)  

## history 객체  
처음 연 이후 사용자의 내비게이션 히스토리를 보관합니다.  
history는 window의 프로퍼티이므로 브라우저, 창, 탭, 프레임은 각각 자신의 history 객체를 가집니다.  

{% highlight js %}
// 한 페이지 뒤로
history.go(-1);
history.back();

// 한 페이지 앞으로
history.go(1);
history.forward();

// 두 페이지 앞으로
history.go(2);

if (history.length == 0) {
  // 사용자가 이 페이지를 처음 열었을 때 실행할 코드
}

{% endhighlight %}

##참고자료  
Nicholas C. Zakas. (2013). 프론트엔드 개발자를 위한 자바스크립트 프로그래밍, (한선용 옮김). 인사이트  
[WEB.BLAZONRY](http://www.blazonry.com/javascript/windows.php)  
[w3schools](http://www.w3schools.com/jsref/obj_location.asp)  
[MDN WEB API](https://developer.mozilla.org/en-US/docs/Web/API)  
