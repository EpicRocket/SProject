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

<img src="./Resources/git_logo.png" width="200px" height="200px" title="git_logo" alt="git_logo"></img>

## [Git(설치)](https://git-scm.com/download/win)

![git_install_step_1](/Resources\Git-2.39.2-64-bit.tmp_lM2JTvTslx.png)
```
Git에 대해 잘 모를 경우 설치 옵션으로 모두 체크 바랍니다.
```
![git_install_step_2](/Resources\Git-2.39.2-64-bit.tmp_tsS2ORnw4O.png)
```
Visual Studio Code를 설치 하셨다면 위 그림과 같은 콤보 박스 옵션을 선택해 주세요.
```

## Git GUI 툴
> 툴은 개인적인 거라 툴 없이 커맨드로만 이용이 가능하지만
> GUI가 있는 편이 좋습니다.

### [Git Extensions](https://github.com/gitextensions/gitextensions/releases/)
![git_extensions_preview](/Resources\GitExtensions_Jp5voCAT0b.png)

### [Source Tree](https://www.sourcetreeapp.com/enterprise)
![source_tree_preview](/Resources\image2018-6-7_15-46-10.png)

### [Git Desktop]()
[Document](https://docs.github.com/ko/desktop/installing-and-configuring-github-desktop/installing-and-authenticating-to-github-desktop/setting-up-github-desktop)
![git_desktop_preview](/Resources\github-desktop-screenshot-windows.png)

### [그외 GUI 툴](https://git-scm.com/downloads/guis)

<br/>

<img src="./Resources\visualstudiologo.png" width="200px" height="200px" title="git_logo" alt="git_logo"></img>
## [Visual Studio(Community 버전 설치)](https://visualstudio.microsoft.com/ko/thank-you-downloading-visual-studio/?sku=Community&channel=Release&version=VS2022&source=VSLandingPage&cid=2030&passive=false)

<br/><br/>


![source_tree_preview](/Resources\iDUkeFFiOo.png)
```
빨간 박스는 모두 체크해주세요.
```

---
## 설치

<br/>

<img src="./Resources\ue-logo-stacked-unreal-engine-w-677x545-fac11de0943f.png" width="248px" height="200px" title="git_logo" alt="git_logo"></img>

## 클라이언트 (Unreal Engine 5)
> 1. ```상대 경로/Client/Setup.bat``` 을 실행 해주세요.
> 2. `Setup.bat` 실행 시 언리얼 엔진을 다운 받습니다.
> 3. 설치가 모두 완료 되었다면 ```상대 경로/Client/GenerateProjectFiles.bat``` 을 실행해 주세요.
> 4. `GenerateProjectFiles.bat` 실행이 완료 되었다면 Client 경로에 `.sln` 파일이 생성 됩니다.
> 5. `.sln` 파일을 `Visual Studio` 를 통해 실행해 주세요.

> 이하 내용은 내가 직접 하면서 필요할듯...

