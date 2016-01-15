---
layout: article
title: "AngularJS 성능개선"
date: 2016-01-14 14:07:13
image:
  teaser: angularjs.png
categories: technology
---

# 개요
이 글은 AngularJS의 웹 어플리케이션 성능개선을 진행한 방법을 정리한 글입니다.  
- AngularJS v1.4.8 기준으로 작성되었습니다.

AngularJS에서 가장 큰 장점은 `양방향 바인딩`이라고 생각합니다. 하지만 이것은 성능에 큰 문제를 주는 병목으로 작용하기도 합니다.  
양방향 바인딩을 구현하기 위해서 AngularJS는 binding되는 변수들을 모두 `$watch`하고 있습니다. 그리고 `$digest` 시에 `$watch`한 변수 중 수정된 것은 돔에 반영해주는 형태를 띄고 있습니다.  

이 `$watch`하는 변수가 적을 때에는 개발도 편하고 성능에도 이상없지만 `$watch`개수가 늘어나면서 성능에 문제가 발생하기 시작합니다. 이번 글도 이 `$watch`와 `$digest` 수를 줄이는 게 주요 내용입니다.  

<figure>
	<a href="https://docs.angularjs.org/guide/databinding#data-binding-in-angular-templates"><img src="//kbs0327.github.io/blog/images/two_way_data_binding.png"></a> 
	<figcaption>AngularJS 양방향 바인딩 개요도</figcaption>
</figure>

#### 주요내용  
- [$httpProvider.useApplyAsync](#httpprovideruseapplyasynchttpsdocsangularjsorgapingproviderhttpprovideruseapplyasync)  
- [$compileProvider.debugInfoEnabled](#compileproviderdebuginfoenabledhttpsdocsangularjsorgapingprovidercompileproviderdebuginfoenabled)  
- [one time binding](#one-time-bindinghttpsdocsangularjsorgguideexpressionhow-to-benefit-from-one-time-binding)  
- [ngRepeat track by](#ngrepeat-track-byhttpsdocsangularjsorgapingdirectivengrepeattracking-and-duplicates)  
- [one time binding과 ngRepeat track by를 조합하여 성능 개선하기](#one-time-binding-ngrepeat-track-by---)  

# [$httpProvider.useApplyAsync](https://docs.angularjs.org/api/ng/provider/$httpProvider#useApplyAsync)  
AngularJS의 홈페이지에서는 이 메소드에 대해 아래처럼 설명하고 있습니다.  
  
> Configure $http service to combine processing of multiple http responses received at around the same time via $rootScope.$applyAsync.  
> This can result in significant performance improvement for bigger applications that make many HTTP requests concurrently (common during application bootstrap).  
  
간단히 요약하면 `$http` 서비스에서 HTTP request의 응답이 올 때에 `$digest`를 호출하는데 동시에 많은 HTTP request를 보낼 때에 `$digest`를 1번만 보내게 한다는 것입니다.  
즉, 이 속성을 키면 `$digest` 호출 수를 줄여서 성능을 개선할 수 있습니다.   

# [$compileProvider.debugInfoEnabled](https://docs.angularjs.org/api/ng/provider/$compileProvider#debugInfoEnabled)  
Runtime 시에 디버깅 모드를 키고 끌지 정하는 메소드입니다.  
이 메소드의 default값은 true이기 때문에 따로 설정하지 않는다면 Real 환경에서도 디버깅 모드로 동작하게 됩니다.  
디버깅 모드에서는 dom을 조작하는 경우(class 변수 수정 등)가 많이 발생하기 때문에 성능저하를 일으켜서 이 속성을 꺼서 성능을 개선할 수 있습니다.  
  
# [one time binding](https://docs.angularjs.org/guide/expression#how-to-benefit-from-one-time-binding)
AngularJS의 가장 큰 장점 중의 하나인 HTML에서 JS변수를 보여줄 수 있는 `Expression`은 `{% raw  %}{{변수명}}{% endraw %}`의 형태로 JS의 변수를 추가할 수 있습니다.

아래와 같이 입력시에  

{% highlight html %}  
<div ng-controller="MyCtrl">  
{% raw  %}
	{{myName}}  
{% endraw %}

</div>  
{% endhighlight %}  
{% highlight js %}  
angular.module('testModule', []).controller('MyCtrl', ['$scope, $timeout', function ($scope, $timeout) {  
	$scope.myName = '김부승';  
	$timeout(function () {  
		$scope.myName = '알투';  
	}, 3000);  
}]);  
{% endhighlight %}  
아래와 같은 형태로 변합니다.  

{% highlight html %}  
<div ng-controller="MyCtrl">  
	김부승  
</div>  
{% endhighlight %}  
그리고 3초 후에 아래처럼 변경됩니다.  

{% highlight html %}  
<div ng-controller="MyCtrl">  
	알투  
</div>  
{% endhighlight %}  

이렇게 변하는 이유는 myName 변수가 변했는지 `$watch`하고 있으며 3초 후에 `$digest`될 때에 myName변수가 변경된 것이 DOM에 반영되기 때문입니다.  
AngularJS는 이렇게 DOM에 `$watch` 수를 줄이기 위해 one time binding을 제공하는데 이것은 1번만 변수를 DOM에 반영하고 `$watch`를 해제시키는 것입니다.  
one time binding을 적용하면 위의 예제는 아래와 같이 변합니다.  
{% highlight html %}  
<div ng-controller="MyCtrl">  
{% raw  %}
	{{::myName}}  
{% endraw %}
</div>  
{% endhighlight %}  
{% highlight js %}  
angular.module('testModule', []).controller('MyCtrl', ['$scope, $timeout', function ($scope, $timeout) {  
	$scope.myName = '김부승';  
	$timeout(function () {  
		$scope.myName = '알투';  
	}, 3000);  
}]);  
{% endhighlight %}  

이렇게 입력하면 똑같이 처음에는 아래처럼 변합니다.  
{% highlight html %}  
<div ng-controller="MyCtrl">  
	김부승  
</div>  
{% endhighlight %}  

하지만 3초 후에 myName변수가 변한 이후에도 아래처럼 보입니다.  
{% highlight html %}  
<div ng-controller="MyCtrl">  
	김부승  
</div>  
{% endhighlight %}  

3초후의 myName 변수는 '알투'라고 저장되어 있지만 DOM에는 반영되지 않습니다. 이것은 `$digest`되도 myName변수를 `$watch`하고 있지 않기 때문에 DOM이 변경되지 않는 것입니다.  

# [ngRepeat track by](https://docs.angularjs.org/api/ng/directive/ngRepeat#tracking-and-duplicates)

## ngRepeat
ngRepeat는 간단하게 표한하자면 HTML 소스에서 for문을 실행시키는 것과 비슷합니다.  

이 ngRepeat는 html소스를 반복하는 형태이고 반복된 HTML에 `$watch`해야 하는 변수를 늘리면 반복하는 수만큼 `$watch`수가 늘어납니다.  
즉, ngRepeat이 n번 반복되고 반복하는 곳에 `$watch`해야 하는 변수가 m개 있다면 1번 `$digest`를 할 때에 `n*m`번의 확인을 하게 됩니다.  
이것은 성능의 저하를 일으키는 주된 원인이 됩니다.  

예를들어 아래와 같이 ngRepeat을 사용하면(변화를 보여주기 위해 one time binding을 사용하겠습니다.)
{% highlight html %}  
<div ng-controller="MyCtrl">  
	<div ng-repeat="member in memberList">  
{% raw  %}
		{{::member.name}}  
{% endraw %}
	</div>  
</div>  
{% endhighlight %}  

{% highlight js %}  
angular.module('testModule', []).controller('MyCtrl', ['$scope, $timeout', function ($scope, $timeout) {  
	$scope.memberList = [{  
		id: 1,  
		name: '김부승',  
		updatedAt: '2015-01-01',  
		organization: 'NHN Entertainment'  
	}, {  
		id: 2,  
		name: '박현재',  
		updatedAt: '2016-01-01',  
		organization: 'NHN Entertainment'  
	}];  
  
	$timeout(function () {  
		$scope.memberList.splice(0, 1, {id: 1, name: '알투'});  
	}, 3000);  
}]);  
{% endhighlight %}  
처음에는 아래와 같은 형태로 보이게 되고  
{% highlight html %}  
<div ng-controller="MyCtrl">
	<div ng-repeat="member in memberList">
		김부승
	</div>
	<div ng-repeat="member in memberList">
		박현재
	</div>
</div>
{% endhighlight %}  

3초 후에는 아래처럼 변하게 됩니다.  
{% highlight html %}  
<div ng-controller="MyCtrl">  
	<div ng-repeat="member in memberList">  
		알투  
	</div>  
	<div ng-repeat="member in memberList">  
		박현재  
	</div>  
</div>  
{% endhighlight %}  

## track by 속성
ngRepeat에는 track by 속성을 주어서 성능을 개선할 수 있습니다.  
track by 속성을 추가하면 해당 변수(혹은 함수)를 계산하여 값이 같으면 DOM을 다시 그리지 않고 기존의 DOM을 가져다가 사용합니다.  

{% highlight html %}  
<div ng-controller="MyCtrl">  
	<div ng-repeat="member in memberList track by member.id">  
{% raw  %}
		{{::member.name}}  
{% endraw %}
	</div>  
</div>  
{% endhighlight %}  

{% highlight js %}  
angular.module('testModule', []).controller('MyCtrl', ['$scope, $timeout', function ($scope, $timeout) {  
	$scope.memberList = [{  
		id: 1,  
		name: '김부승',  
		updatedAt: '2015-01-01',  
		organization: 'NHN Entertainment'  
	}, {   
		id: 2,  
		name: '박현재',  
		updatedAt: '2016-01-01',  
		organization: 'NHN Entertainment'  
	}];  
  
	$timeout(function () {  
		$scope.memberList.splice(0, 1, {id: 1, name: '알투'});  
	}, 3000);  
}]);  
{% endhighlight %}  
  
이렇게 수정하면 처음에는 아래와 같은 형태로 보이게 되고  
{% highlight html %}  
<div ng-controller="MyCtrl">  
	<div ng-repeat="member in memberList track by member.id">  
		김부승  
	</div>  
	<div ng-repeat="member in memberList track by member.id">  
		박현재  
	</div>  
</div>  
{% endhighlight %}  

하지만 3초 후에 myName변수가 변한 이후에도 아래처럼 보입니다.  
{% highlight html %}  
<div ng-controller="MyCtrl">  
	<div ng-repeat="member in memberList track by member.id">  
		김부승  
	</div>  
	<div ng-repeat="member in memberList track by member.id">  
		박현재  
	</div>  
</div>  
{% endhighlight %}  

이렇게 `$digest`되더라도 ngRepeat에서 track by 속성을 통해 DOM 전체를 다시 그릴지 판단하는데 변화를 감지하는데,  
member.id가 변하지 않아서 해당 돔 전체를 그대로 사용하기 때문에 이전과 똑같이 보이게 됩니다.  

### track by 사용 시에 주의할 점
track by를 사용할 때에는 track by속성이 있는 ngRepeat에서 track by의 결과가 중복되서는 안된다는 것입니다.  
만약 위의 예에서 아래처럼 member.organization을 사용하면 멤버 2명의 값이 같기 때문에 에러가 발생합니다.  
{% highlight html %}  
<div ng-controller="MyCtrl">  
	<div ng-repeat="member in memberList track by member.organization">  
{% raw  %}
		{{::member.name}}  
{% endraw %}
	</div>  
</div>  
{% endhighlight %}  

# one time binding과 ngRepeat track by를 조합하여 성능 개선하기  
one time binding과 ngRepeat의 track by 속성을 조합하면 서로의 단점을 보완하여 성능을 개선할 수 있습니다.  
one time binding은 DOM을 그릴 때에 값을 1번만 binding하고 `$watch`하지 않게 해제하는 방법을 사용하고,  
ngRepeat의 track by같은 경우에는 track by 값이 변하면 DOM을 다시 그린다는 특징이 있습니다.  

이 2개의 특징을 조합하면 ngRepeat의 track by 값을 변하게 하면 one time binding한 변수들의 값을 수정할 수 있게 됩니다.  
예를 들어서 위의 track by 예제에서 track by를 (member.id + member.updatedAt)으로 하면  
member.updatedAt으로 값이 변경되었는지 확인하고 member.id로 track by의 유일성 조건을 맞족하게 됩니다.
이를 코드로 표현하면  
{% highlight html %}  
<div ng-controller="MyCtrl">  
	<div ng-repeat="member in memberList track by (member.id + member.updatedAt)">  
{% raw  %}
		{{::member.name}}  
{% endraw %}
	</div>  
</div>  
{% endhighlight %}  

{% highlight js %}  
angular.module('testModule', []).controller('MyCtrl', ['$scope, $timeout', function ($scope, $timeout) {  
	$scope.memberList = [{  
		id: 1,  
		name: '김부승',  
		updatedAt: '2015-01-01',  
		organization: 'NHN Entertainment'  
	}, {  
		id: 2,  
		name: '박현재',  
		updatedAt: '2016-01-01',  
		organization: 'NHN Entertainment'  
	}];  

	$timeout(function () {  
		$scope.memberList.splice(0, 1, {id: 1, name: '알투'});  
	}, 3000);  
}]);  
{% endhighlight %}  
위 코드를 실행한 결과는 아래처럼 보이게 됩니다.  

{% highlight html %}  
<div ng-controller="MyCtrl">  
	<div ng-repeat="member in memberList track by (member.id + member.updatedAt)">  
		김부승  
	</div>  
	<div ng-repeat="member in memberList track by (member.id + member.updatedAt)">  
		박현재  
	</div>  
</div>  
{% endhighlight %}  

그리고 3초 후에는 아래처럼 값이 변경되게 됩니다.  
{% highlight html %}  
<div ng-controller="MyCtrl">  
	<div ng-repeat="member in memberList track by (member.id + member.updatedAt)">  
		알투  
	</div>  
	<div ng-repeat="member in memberList track by (member.id + member.updatedAt)">  
		박현재  
	</div>  
</div>  
{% endhighlight %}  
  
이런 형태로 코드를 작성하게 되면 ngRepeat의 track by 속성의 값을 계산하는데는 약간의 시간이 더 걸리겠지만  
ngRepeat 안의 `$watch`되는 변수들을 one time binding을 사용하여 `$watch` 수를 줄일 수 있고 이는 1번 `$digest`할 때의 비용을 줄여줍니다.  

Dooray에서는 위의 방법들을 주로 사용하여 속도를 1초정도 개선할 수 있었습니다.  

# 그 외 성능개선 Tip  
  
### [AngularJS Batarang](https://chrome.google.com/webstore/detail/angularjs-batarang-stable/niopocochgahfkiccpjmmpchncjoapek)
크롬익스텐션으로 이를 사용하면 DOM의 `$watch`변수들의 실행 시간을 확인할 수 있습니다.  

### [watch vs watchCollection 성능 분석](http://plnkr.co/edit/Pbvo6AqME081rZ2RVKDU?p=preview)
성능 개선을 해 보면 `$watch`와 `$watchCollection`의 차이를 알아야합니다. 이 사이트는 이 2개의 차이점을 이해하기 쉽게 해줍니다.  

### [AngularJS 성능개선 Tip 11가지](http://www.alexkras.com/11-tips-to-improve-angularjs-performance/)
AngularJS의 성능개선 시에 Tip을 알려줍니다.

# 참고자료  
[AngularJs 공식 홈페이지](https://angularjs.org/)
