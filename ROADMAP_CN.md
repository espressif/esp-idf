# ESP-IDF 项目路线图 2026

* [English Version](./ROADMAP.md)

本文档概述了 ESP-IDF 项目的年度计划，方便客户据此规划自己的项目周期。需要说明的是该文档并不是我们对客户的约束性承诺。相反，其主要目的是为客户提供 ESP-IDF 项目开发的路线图和方向。通过公开这些信息，我们希望增进客户对 ESP-IDF 项目的理解，提高透明度，并确保与 ESP-IDF 项目的总体目标保持一致。

## 项目总览

### 项目目标

在 ESP-IDF 的主要版本和次要版本中，我们一般会增加对新芯片的支持，以扩展我们的产品线。通过扩展芯片矩阵，拓宽我们的产品范围，并满足广泛受众的各种需求。这样便能保证我们的产品始终处于技术进步的前沿，不断满足客户的需求并超越客户的期望。

此外，ESP-IDF 各活跃分支的 Bugfix 版本发布也是我们项目的重中之重，着力提升已量产产品的稳定性和性能。通过及时解决问题，我们期待提升用户的整体体验，切实惠及使用乐鑫解决方案的客户。通过积极维护 ESP-IDF 的各活跃分支，我们践行了对宝贵的客户群提供可靠、高质量产品的承诺。

以下是 ESP-IDF 项目在 2026 年计划实现的主要目标。

* 新芯片支持

	* 增加对 ESP32-H21 芯片的支持
	* 增加对 ESP32-H4 芯片的支持

* 发布更多的次要和主要版本

	* 在 2026 年第一季度发布 IDF v6.0
	* 在 2026 年中发布 IDF v6.1 
	* 在 2026 年底发布 IDF v6.2

* 发布更多 bugfix 版本

	* 在 2026 年 8 月底 IDF v5.2 停止维护之前，发布 IDF v5.2.7 和 IDF v5.2.8
	* 在 2027 年 1 月底 IDF v5.3 停止维护之前，发布 IDF v5.3.5，v5.3.6 和 IDF v5.3.7
	* 在 release/5.4 分支和 release/5.5 分支进入维护周期之前，发布更多 bugfix 版本
	* release/6.0 分支和 release/6.1 分支发布更多 bugfix 版本，使这两个分支更加稳定和产品化

* 重大变更

	* ESP-IDF v6.0 正在升级至 MbedTLS v4.x 和新的 PSA 加密 API。由于该项工作仍在进行中，因此即将发布的 beta 版本或 RC 版本可能会包含对加密 API 的进一步更新或非兼容性更新。

请注意，获取之前芯片的支持状态，请参阅 [ESP-IDF 发布和 SoC 兼容性](https://github.com/espressif/esp-idf/blob/master/README_CN.md#esp-idf-与乐鑫芯片)。

### 路线图细节

ESP-IDF 项目重视持续维护和更新，确保我们的客户始终处于技术进步的前沿。我们承诺持续进行开发，并将该领域的最新创新成果呈现给客户。

此外，我们也在给客户赋能，客户通过迭代改进便能接触到新开发的功能和更高的性能。我们在突破技术界限方面的坚定承诺，使客户不仅能接触到最新的技术，还能从我们产品的尖端功能中获取最大价值。

以下是 ESP-IDF 路线图的主要信息。

* 新芯片支持

	* 当 ESP32-H21 芯片达到量产状态时，将增加对其量产版本的初步支持。支持进展将通过开发者门户中的[新芯片支持状态页面](https://developer.espressif.com/hardware/) 进行跟踪。
	* 当 ESP32-H4 芯片达到量产状态时，将增加对其量产版本的初步支持。支持进展将通过开发者门户中的[新芯片支持状态页面](https://developer.espressif.com/hardware/) 进行跟踪。

* Bugfix 版本发布

	* 在 2026 年 8 月底 IDF v5.2 停止维护之前，发布 IDF v5.2.7 和 IDF v5.2.8
	* 在 2027 年 1 月底 IDF v5.3 停止维护之前，发布 IDF v5.3.5，v5.3.6 和 IDF v5.3.7
	* release/5.4 分支自 2026 年 1 月进入维护周期，在 2026 年发布 Bugfix 版本 IDF v5.4.4，IDF v5.4.5 
	* release/5.5 分支自 2026 年 7 月进入维护周期，在 2026 年发布 Bugfix 版本 IDF v5.5.3，IDF v5.5.4，IDF v5.5.5，IDF v5.5.6 和 IDF v5.5.7
	* release/6.0 分支和 release/6.1 分支发布更多 bugfix 版本，使 release/6.0 分支和 release/6.1 分支更加稳定和产品化。

## ESP-IDF 发布计划

获取 ESP-IDF 的完整发布列表，请访问 https://github.com/espressif/esp-idf/releases

此处提供的所有信息均可因业务原因及其他因素而在没有通知的情况下进行更改。

### ESP-IDF 主要版本发布

```mermaid
timeline

        title ESP-IDF Major Releases
        section 2026 Q1 <br> Major Release Planning
          v6.0-beta2 : 2026/01/27   
          v6.0-rc1 : 2026/02/22
          v6.0: 2026/02/27     
        section 2026 Q2 <br> Major Release Planning
          No version planned : N/A   
        section 2026 Q3 <br> Major Release Planning
          No version planned : N/A      
        section 2026 Q4 <br> Major Release Planning
          No version planned : N/A 
```
  
### ESP-IDF Minor Releases

```mermaid
timeline

        title ESP-IDF Minor Releases
        section 2026 Q1 <br> Minor Release Planning
          No version planned : N/A               
        section 2026 Q2 <br> Minor Release Planning
          v6.1-beta1 : 2026/06/11   
          v6.1-rc1 : 2026/07/23
          v6.1: 2026/07/31    
        section 2026 Q3 <br> Minor Release Planning
          No version planned : N/A   
        section 2026 Q4 <br> Minor Release Planning
          v6.2-beta1 : 2026/11/11   
          v6.2-rc1 : 2026/12/23
          v6.2: 2026/12/31     
```

### ESP-IDF Bugfix Releases

```mermaid
timeline

        title ESP-IDF Bugfix Releases
        section 2026 Q1 <br> Bugfix Release Planning
          v5.5.3 : 2026/02/13   
          v5.3.5 : 2026/03/09
          v6.0.1 : 2026/04/10
          v5.4.4 : 2026/04/03                  
        section 2026 Q2 <br> Bugfix Release Planning
          v5.2.7 : 2026/04/28
          v5.5.4 : 2026/05/08
          v6.0.2 : 2026/06/04
          v5.3.6 : 2026/07/02    
```

 ```mermaid
timeline

        section 2026 Q3 <br> Bugfix Release Planning
          v5.5.5 : 2026/07/30
          v5.2.8 : 2026/08/17
          v6.1.1 : 2026/09/03   
          v6.0.3 : 2026/09/10
          v5.4.5 : 2026/09/28
        section 2026 Q4 <br> Bugfix Release Planning
          v5.5.6 : 2026/10/08
          v6.1.2 : 2026/10/22
          v6.0.4 : 2026/11/18
          v6.1.3 : 2026/12/16         
          v5.5.7 : 2027/01/11
          v5.3.7 : 2027/01/18   
```