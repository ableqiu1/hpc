## GIT

> window直接下载安装包安装

#### **配置**

```
git config --global user.name "ableqiu1"
git config --global user.email "email"
```

```
--------ssh
// 生成密钥
ssh-keygen -t rsa -C "qacepisode@gmail.com"

// 公钥设置到github
// linux默认在~/.ssh
```



#### 创建一个仓库repository（ 版本库）

```
mkdir learn 
git init learn   //  把这个目录变成Git可以管理的仓库，之后文件会被追踪
```

##### <img src="C:\data\note\HPC\assets\image-20240715153437040.png" alt="image-20240715153437040" style="zoom:50%;" />

<img src="C:\data\note\HPC\assets\image-20240715153913882.png" alt="image-20240715153913882" style="zoom:50%; text-align:left; float:left;" />



#### 常用命令

```
git status 	//查看文件
git add  	// 提交文件到暂存区
	- git add filename,filename2...
	- git add *.txt
	- git add .     // 当前目录全部文件
git commit -m "relation information"   // 提交暂存区的文件到本地仓库，不理会工作区中文件

-------- 提交日志，查看版本编号
git log (--oneline)			// 查看提交日志

-------- 查看差异
git diff   					// 默认查看工作区、暂存区差异
git diff HEAD  				//查看工作区、版本库差异
git diff --cached			//查看暂存区、版本库差异
git diff 版本ID 版本ID2      //查看两个版本库差异
git diff branch_name branch_name2 


-------- 删除文件
git rm filename   // 从工作区和暂存区删除文件，提交后会将版本库中也删除
git rm --cached filename // 从暂存区中删除文件，保留工作区中文件，提交后会将版本库中也删除
git rm -r*   //递归删除某个目录下的所有子目录和文件，删除后提交
```



#### 回退命令

<img src="C:\data\note\HPC\assets\image-20240715160259900.png" alt="image-20240715160259900" style="zoom:50%;float:left;" />

soft模式：本地东西还在，只是git记录改变了，commit记录没了

hard模式：文件没了

mixed模式：文件在，只是git记录改变了，commit和add记录没了



#### 忽略文件.gitignore

- 仓库根目录添加文件.gitignore
- .gitignore文件中记录的文件不会提交
- .gitignore中记录的文件不对版本库中已经存在的文件生效
- 忽略文件夹格式如 `foldername/`
- 匹配规则:
  - #开始的行为注释行,注释行和空行会被忽略
  - `*` 表示匹配多个字符
  - `?` 表示匹配单个字符
  - `[]`表示匹配括号中的单个字符 , [0-9] 表示0到9中的任意一个数字, [a-z]类似
  - `**`两个星号表示匹配任意的中间目录
  - `!` 表示取反



#### 关联本地仓库和远程仓库

> 仓库名不需要相同

<img src="C:\data\note\HPC\assets\image-20240715194808826.png" alt="image-20240715194808826" style="zoom:50%; float:left" />

##### 克隆

`git clone address`

- 克隆后关联的远程仓库默认命名为`origin`

- 默认拉取主分支master/main
- `git checkout -b dev origin/feature` 可将分支之间建立联系，dev为分支名
- `git checkout -t origin/feature` -t参数默认在本地建立一个和远程仓库分支一样的分支
- 上面两种操作都会拉去远程仓库的分支，本地仓库和远程仓库分支之间具有关联关系，可以直接`git push`



##### 建立关联

```
// origin为自定义远程仓库显示名,直接克隆下来的默认为origin 
git remote add origin <address>
```

##### 推送

```
// 推送，建立分支关联
git push -u <remote_name> <local_branch>:<remote_branch>  
```

##### 拉取

```
// remote_name branch_name可省略，默认为origin和主分支
// 拉取后会合并

git pull <remote_name> <branch_name>
```



#### 分支相关

##### 基本操作

```
// 获取当前分支名 
git branch    

// 获取全部分支，包括远程仓库的分支
git branch -a

// 创建一个新的分支，不切换
git branch <new_branch_name>  

// 切换分支
git checkout <branch_name>
git switch <branch_name>

// 删除分支
git branch -d branch_name
git branch -D branch_name  // 强制删除未合并的分支

// 合并分支,在目标分支上
git merge dev   // 将dev分支合并到目标分支
```



##### 合并冲突

1. 使用merge合并分支时可能会产生冲突，这时需要手动修改产生冲突的文件然后再add和commit
2. 还可以使用rebase合并分支