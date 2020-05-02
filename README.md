## 一些demo
---
[扫雷游戏 Demo 地址](Bombsweep/Release)  

### 0. Note
---
难度中等/困难解锁30 bombs左右有bug仍未解决  
另外在游戏开始之后，频繁拖动边框也会出现bug

### 1. 界面设计
---
难度分为简单(10 bombs)、中等(40 bombs)和困难(100 bombs)  
<div align="center"><img  src="images/hardness-easy.png" width="35%" height="35%" /></div>  
<div align="center"><img  src="images/hardness-midium.png" width="35%" height="35%" /></div>  
<div align="center"><img  src="images/hardness-rough.png" width="35%" height="35%" /></div>  
支持界面边界拖动，其内元素会自适应变化  
<div align="center"><img  src="images/ui-adaptive-change.png" width="35%" height="35%" /></div>  

### 2. 其他设计
---
第一次点击不出现bomb。另外在点击到空白块的同时会暴露周围所有的空白块  
<div align="center"><img  src="images/fst-click.png" width="35%" height="35%" /></div>  
点击到bomb时..  
<div align="center"><img  src="images/click-bomb.png" width="35%" height="35%" /></div>  