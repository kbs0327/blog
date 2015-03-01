---
layout: post
title:  "상속과 인터페이스 사용 예"
date:   2015-03-01 15:27:13
image: 
  teaser: keyboard.jpg
categories: technology
---
  
 이 글은 백창열 수석님의 코드리뷰를 바탕으로 정리한 내용입니다.  

## 문제의 코드

 파일첨부가 가능한 노트와 댓글을 쓰는 프로그램을 작성한다고 하면, 아래와 같이 코드가 나올 수 있습니다.
{% highlight java %}
public class NoteDto {
	...
	String attachedFileName;
	
	private final String[] bannedFileExtentionList = {"exe"};
	
	...
	public String getAttachedFileName() {
		return attachedFileName;
	}

	public void setAttachedFileName(String attachedFileName) {
		this.attachedFileName = attachedFileName;
	}
	
	public boolean isValidFileName() {
		return attachedFileName!= null && !hasBannedFileExtention();
	}
}

public class CommentDto {
	...
	String attachedFileName;
	
	private final String[] bannedFileExtentionList = {"exe"};
	
	...
	public String getAttachedFileName() {
		return attachedFileName;
	}

	public void setAttachedFileName(String attachedFileName) {
		this.attachedFileName = attachedFileName;
	}
	
	public boolean isValidFileName() {
		return attachedFileName!= null && !hasBannedFileExtention();
	}
}
{% endhighlight %}
 중복되는 부분만 나타냈습니다. 그리고 저는 프로그램을 만들 때 중복을 최소화 하는 것이 좋다고 배워서,
아래와 같이 리팩토링 했습니다.

{% highlight java %}
public class NoteDto extends FileDto {
	...
}

public class CommentDto extends FileDto {
	...
}

public class FileDto {
	String attachedFileName;
	
	private final String[] bannedFileExtentionList = {"exe"};
	
	...

	public String getAttachedFileName() {
		return attachedFileName;
	}

	public void setAttachedFileName(String attachedFileName) {
		this.attachedFileName = attachedFileName;
	}
	
	public boolean isValidFileName() {
		return attachedFileName!= null && !hasBannedFileExtention();
	}
}
{% endhighlight %}
 훌륭하게 중복을 줄였습니다. 
 하지만 이 코드는 많은 문제점을 발생시킬 수 있습니다.  
1. 노트와 댓글에 파일 말고 이메일 정보가 들어간다고 하면 어떻게 해야할까요?  
2. 코드를 보는 사람이 정의 부분을 보지 않고 파일을 상속한다고 생각할 수 있을까요?

1번째 문제는 자바에서 상속은 1개의 클래스에서만 가능하게 해서 문제가 생깁니다.
그러면 자바의 설계의 문제일까요?

2번째 문제를 보면 보는 사람이 정의를 제대로 보지 않아서 문제가 발생한 것일까요?

먼저 상속의 원칙에 대해서 알아보겠습니다.

## 상속
  
 상속을 사용할 때에는 항상 is-A관계인지를 생각해야 합니다.
 여기서 is-A관계란 민호의 말처럼 우리나라 말로 은, 는 법칙과 같은 뜻으로 쓰이는 것 같습니다.

 예를 들어 보면 아래같이 티라미슈노트라는 클래스는 노트라는 클래스를 상속받을 수 있습니다.
{% highlight java %}
public class TiramisuNoteDto extends NoteDto {
   ...
}
{% endhighlight %}
티라미슈노트가 노트라는 것은 확실한 사실입니다.

 하지만 상속을 할 때는 중복을 줄이기 위해서, 혹은 편의를 위해서 위에서 나온 것 같은 실수를 합니다.
{% highlight java %}
public class NoteDto extends FileDto {
   ...
}

public class CommentDto extends FileDto {
   ...
}
{% endhighlight %}
 그렇기 때문에 코드를 처음보는 사람이 정의를 보지 않고서는 상속관계를 유추하기 힘든 것입니다.

 하지만 상속을 사용하지 않으면 노트와 댓글 안에는 파일과 관련된 내용들이 중복되어서 적용되어 버립니다.

## 유틸리티 클래스로 분리

 이런경우에 많이 사용하는 것이 유틸리티 클래스입니다. 아래와 같이 유틸리티 클래스로 분리할 수 있습니다.
{% highlight java %}
public class NoteDto {
	String attachedFileName;
	
	...

	public String getAttachedFileName() {
		return attachedFileName;
	}

	public void setAttachedFileName(String attachedFileName) {
		this.attachedFileName = attachedFileName;
	}

	public boolean isValidFileName() {
		return FileUtil.isValidFileName(this);
	}
}

public class CommentDto {
	String attachedFileName;
	
	...

	public String getAttachedFileName() {
		return attachedFileName;
	}

	public void setAttachedFileName(String attachedFileName) {
		this.attachedFileName = attachedFileName;
	}

	public boolean isValidFileName() {
		return FileUtil.isValidFileName(this);
	}
}

public class FileUtil {
	private final String[] bannedFileExtentionList = {"exe"};
	
	public static boolean isValidFileName(NoteDto note) {
		String attachedFileName = note.getAttachedFileName();
		return attachedFileName!= null && !hasBannedFileExtention(attachedFileName);
	}
	
	public static boolean isValidFileName(CommentDto comment) {
		String attachedFileName = comment.getAttachedFileName();
		return attachedFileName!= null && !hasBannedFileExtention(attachedFileName);
	}
	
	...
}
{% endhighlight %}

이렇게 유틸리티 클래스를 이용해서 분리할 수 있습니다.
하지만 이렇게 하면 파일이름의 유효성 확인을 하는 클래스가 늘어나면 그 늘어난 수만큼 유틸리티 클래스가 길어집니다.

그러면 과연 이 방법이 최선일까요?

## 인터페이스

 인터페이스는 구현한 클래스가 구현해야할 메소드들을 정리한 명세서입니다.

 위의 코드를 인터페이스를 사용해 중복을 없애보겠습니다.
{% highlight java %}
public class NoteDto implements File {
	String fileName;
	
	...

	@Override
	public String getFileName() {
		return fileName;
	}

	@Override
	public void setFileName(String fileName) {
		this.fileName = fileName;
	}
	
	@Override
	public boolean isValidFileName() {
		return FileUtil.isValidFileName(this);
	}
}

public class CommentDto implements File {
	String fileName;
	
	...

	@Override
	public String getFileName() {
		return fileName;
	}

	@Override
	public void setFileName(String fileName) {
		this.fileName = fileName;
	}
	
	@Override
	public boolean isValidFileName() {
		return FileUtil.isValidFileName(this);
	}
}

public interface File {
	String getFileName();
	void setFileName(String FileName);
	boolean isValidFileName();
}

public class FileUtil {
	private final String[] bannedFileExtentionList = {"exe"};
	
	public static boolean isValidFileName(File file) {
		String fileName = file.getFileName();
		return fileName!= null && !hasBannedFileExtention(fileName);
	}
	
	...
}
{% endhighlight %}
이렇게 해서 리팩토링을 끝냈습니다.
위와 같은 방식으로 프로그램을 만들면 아래와 같은 장점이 있습니다.  
1. 코드 중복의 최소화  
2. 이해하기 쉬운 코드 작성  

**이미지 출처**  
[http://www.freestockphotos.biz/stockphoto/1770](http://www.freestockphotos.biz/stockphoto/1770)  
