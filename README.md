Sub Project
======================
---
## 목차
- 요구 사항
  * **[git 설치](https://git-scm.com/download/win)** <sub>[가이드](GUIDEGIT.md)</sub>
    - Github 계정
  * **[Visual Studio 설치](https://visualstudio.microsoft.com/ko/thank-you-downloading-visual-studio/?sku=Community&channel=Release&version=VS2022&source=VSLandingPage&cid=2030&passive=false)** <sub>[가이드](GUIDEVS.md)</sub>
    -  Microsoft 계정
  * **[언리얼엔진](https://github.com/EpicRocket/UnrealEngine.git)**
  - <U>선택(권장)</U>
    * **[Visual Studio code 설치](https://code.visualstudio.com/docs/?dv=win64user)** <sub>[가이드](GUIDEVSCODE.md)</sub>&ensp;|&ensp;**[Notepad++ 설치](https://notepad-plus-plus.org/downloads/)**

- [설치 가이드](#install)
****

<h2 id="install">설치 가이드</h2>

![alt text](Resources/explorer_qRiR0ggrth.gif)
1. 아래 명령어를 cmd에 입력해줍니다.
```shell
git clone https://github.com/EpicRocket/SProject.git
```

![alt text](Resources/explorer_glKpzuHqPC.gif)
2. `SProject.uproject` 파일을 우클릭하여 GeneratedProjectFiles을 실행시켜줍니다.

![alt text](Resources/explorer_cOLB19AsYF.png)
3. `SProject.sln`를 실항하여 Visual Studio를 실행시킵니다.

![alt text](Resources/devenv_9rMnIui414.gif)
4. 솔루션 탐색기에서 `SProject`를 우클릭하여 `시작 프로젝트로 설정`을 클릭합니다.

![alt text](Resources/devenv_hGb0nPpB76.gif)
5. `Development Editor`로 변경해줍니다.
 
![alt text](Resources/devenv_LEPucdzwPe.gif)
6. `디버그(D)`메뉴에서 `디버그하지 않고 시작`을 클릭해 게임 프로젝트를 켤 수 있습니다.
   - 빌드가 완료되었다면, `SProject.uproject`를 실행하여 프로젝트를 켤 수 있습니다. 하지만 소스파일 변경 시 `빌드`가 필요합니다. <sub>추후 스토리지가 생긴다면 바이너리를 통해 업데이트 받을 수 있으면 좋겠습니다.</sub>