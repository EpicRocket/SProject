Sub Project
======================
```
사용 엔진 : Unreal Engine
버전 : 5.1
마지막 업데이트 내역
04f3a110a8feea1f45f7f7a83b2039ee058cfecc
Localization Automation using CL 24024253
[CL 24026516 by buildmachine in 5.1 branch]
```
---
## 목차
- 요구 사항
    - Git
        - Github 계정
    - Visual Studio 2017, 2019, 2022 or latest version
        - Microsoft 계정
    - <U>선택(권장)</U>
        - [Visual Studio Code(설치)](https://code.visualstudio.com/download)
        - GUI 툴(SourceTree, GitDesktop, GitExtensions ...)
- 설치
    - 클라이언트

<br/><br/>

## * 요구 사항
> 가능 하다면 <U>선택(권장)</U> 프로그램을 을 먼저 설치하고 진행 하시길 바랍니다.

<br/>

<div style = "text-align:center";>

<img src="./Resources/git_logo.png" width="200px" height="200px"></img>

## [Git(설치)](https://git-scm.com/download/win)

</div>

<br/>

<img src="./Resources\Git-2.39.2-64-bit.tmp_lM2JTvTslx.png"></img>
```
Git에 대해 잘 모를 경우 설치 옵션으로 모두 체크 바랍니다.
```

<img src="./Resources\Git-2.39.2-64-bit.tmp_tsS2ORnw4O.png"></img>
```
Visual Studio Code를 설치 하셨다면 위 그림과 같은 콤보 박스 옵션을 선택해 주세요.
```

## Git GUI 툴
> 툴은 개인적인 거라 툴 없이 커맨드로만 이용이 가능하지만
> GUI가 있는 편이 좋습니다.

### [Git Extensions](https://github.com/gitextensions/gitextensions/releases/)
<img src="./Resources\GitExtensions_Jp5voCAT0b.png"></img>

### [Source Tree](https://www.sourcetreeapp.com/enterprise)
<img src="./Resources\image2018-6-7_15-46-10.png"></img>

### [Git Desktop]()
[Document](https://docs.github.com/ko/desktop/installing-and-configuring-github-desktop/installing-and-authenticating-to-github-desktop/setting-up-github-desktop)

<img src="./Resources\github-desktop-screenshot-windows.png"></img>

### [그외 GUI 툴](https://git-scm.com/downloads/guis)

<br/>

<img src="./Resources\visualstudiologo.png" width="200px" height="200px"></img>
## [Visual Studio(Community 버전 설치)](https://visualstudio.microsoft.com/ko/thank-you-downloading-visual-studio/?sku=Community&channel=Release&version=VS2022&source=VSLandingPage&cid=2030&passive=false)

<br/>

<img src="./Resources\iDUkeFFiOo.png"></img>
```
빨간 박스는 모두 체크해주세요.
```

---
## 설치

<br/>

<div style = "text-align:center";>

<img src="./Resources\ue-logo-stacked-unreal-engine-w-677x545-fac11de0943f.png" width="248px" height="200px"></img>

## 클라이언트 (Unreal Engine 5)

</div>

> 1. ```상대 경로/Client/Setup.bat``` 을 실행 해주세요.
> 2. `Setup.bat` 실행 시 언리얼 엔진을 다운 받습니다.
> 3. 설치가 모두 완료 되었다면 ```상대 경로/Client/GenerateProjectFiles.bat``` 을 실행해 주세요.
> 4. `GenerateProjectFiles.bat` 실행이 완료 되었다면 Client 경로에 `UE5.sln` 파일이 생성 됩니다.
> 5. `UE5.sln` 파일을 `Visual Studio` 를 통해 실행해 주세요.
>
> <div style = "text-align:center";> 
> <img src="./Resources\XNSTUbBYiJ.png"></img>
> </div>
>
> 6. 상단 콤보 박스에서 `Development Editor` `Wint64` 로 변경해주 세요.
>
> <div style = "text-align:center";>
> <img src="./Resources\g4LeScfWuX.png"></img>
> </div>
>
> 7. 솔루션 탐색기 탭을 열고 `Game/SProject` 를 우클릭 후 `시작 프로젝트로 설정(A)` 를 클릭해주세요.
> 8. 마지막으로 `Ctlr + F5` 키를 눌러 언리얼을 시작하시면 됩니다.
> 9. 빌드가 완료 되어야 언리얼이 시작이 됩니다. 가능한 빌드를 도중에 멈춰주지 말아주세요.

