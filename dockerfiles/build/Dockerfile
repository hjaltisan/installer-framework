# escape=`

FROM microsoft/windowsservercore:1803 AS builder

# Switch to Powershell for utility installs
SHELL ["powershell", "-Command"]

# Install Chocolatey
RUN Set-ExecutionPolicy Bypass -Scope Process -Force; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))

# Install required packages for configuration and compilation of Qt and IFW
RUN choco install --yes --no-progress --limit-output git python2 activeperl jom

WORKDIR C:\src

# Revert to default shell
SHELL ["cmd", "/S", "/C"]

# Set up build-tools
ADD https://aka.ms/vs/15/release/vs_buildtools.exe C:\TEMP\vs_buildtools.exe
RUN C:\TEMP\vs_buildtools.exe --quiet --wait --norestart --nocache `
    --installPath C:\BuildTools `
    --add Microsoft.VisualStudio.Component.VC.CoreBuildTools `
    --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
    --add Microsoft.VisualStudio.Component.Windows10SDK.17134 `
    --add Microsoft.VisualStudio.Component.VC.CMake.Project `
    --add Microsoft.VisualStudio.Component.VC.ATL `
    --add Microsoft.VisualStudio.ComponentGroup.NativeDesktop.Win81 `
    --add Microsoft.Component.ClickOnce.MSBuild `
 || IF "%ERRORLEVEL%"=="3010" EXIT 0

RUN echo "##teamcity[blockOpened name='qt5-clone' description='Clone qt5 repository']"

RUN git clone --progress --verbose https://github.com/qt/qt5.git

WORKDIR C:\src\qt5

RUN git checkout v5.11.3

RUN echo "##teamcity[blockClosed name='qt5-clone']" `
 && echo "##teamcity[blockOpened name='qt5-modules-clone' description='Clone qt5 module repositories']"

# git submodules not working in Windows containers. Manually syncing modules instead.

RUN git clone --progress --verbose https://github.com/qt/qtbase.git
RUN cd C:\src\qt5\qtbase `
 && git checkout v5.11.3

RUN git clone --progress --verbose https://github.com/qt/qtdeclarative.git
RUN cd C:\src\qt5\qtdeclarative `
 && git checkout v5.11.3

RUN git clone --progress --verbose https://github.com/qt/qtwinextras.git
RUN cd C:\src\qt5\qtwinextras `
 && git checkout v5.11.3

RUN git clone --progress --verbose https://github.com/qt/qttools.git
RUN cd C:\src\qt5\qttools `
 && git checkout v5.11.3

RUN git clone --progress --verbose https://github.com/qt/qtmultimedia.git
RUN cd C:\src\qt5\qtmultimedia `
 && git checkout v5.11.3

RUN echo "##teamcity[blockClosed name='qt5-modules-clone']" `
 && echo "##teamcity[blockOpened name='qt5-build' description='Build Qt5']"

RUN C:\BuildTools\VC\Auxiliary\Build\vcvarsall.bat x86 10.0.17134.0 `
 && configure -opensource -confirm-license -prefix c:\build\qt5 -release -static -static-runtime -platform win32-msvc -accessibility -no-opengl -no-icu -no-sql-sqlite -no-qml-debug -nomake examples -nomake tests -skip qtactiveqt -skip qtenginio -skip qtlocation -skip qtmultimedia -skip qtserialport -skip qtquickcontrols -skip qtscript -skip qtsensors -skip qtwebengine -skip qtwebsockets -skip qtxmlpatterns -skip qt3d
RUN C:\BuildTools\VC\Auxiliary\Build\vcvarsall.bat x86 10.0.17134.0 `
 && jom `
 && jom install

RUN echo "##teamcity[blockClosed name='qt5-build']" `
 && echo "##teamcity[blockOpened name='qt5-ifw-build' description='Build Qt5 Installer Framework']"

# Get the installer-framework source
COPY . c:\src\ifw
WORKDIR c:\src\ifw

RUN C:\BuildTools\VC\Auxiliary\Build\vcvarsall.bat x86 10.0.17134.0 `
 && C:\build\qt5\bin\qmake.exe `
 && jom `
 && jom install

#RUN nmake
# && jom install

#RUN echo "##teamcity[blockClosed name='qt5-ifw-build']"

#FROM microsoft/windowsservercore:1803 AS runner

#COPY --from=builder C:\build\qt5 C:\build\qt5

# Installer configuration location
#VOLUME C:\src\installer

#ENTRYPOINT c:\build\qt5\binarycreator.exe -f -v -c c:\src\installer\config\config.xml -p c:\src\installer\packages C:\src\installer\installer.exe