# 请在仅使用numpy/pandas的前提下，实现一个Deterministic User-Mean Baselinde用于评分预测。对给定(user, item)测试对，输出预测评分（保留两位小数），算法要求简单却体现推荐系统常见的缺失值填补、冷启动逻辑。 预测规则：1.若用户u在训练集中已评价item i，直接返回该真实评分；2.否则若用户u有其他历史评分，参考用户历史平均评分；3.否则（冷启动用户）。以全局平均评分代替个体分数；4.结果四舍五入保留2位小数，已字符串形式输出，限定区间[1.00, 5.00]。备注：训练集中不会出现重复的(user, item)，评分均在1-5之间。 输入描述：输入格式 { “train”:[[user_id, item_id, rating], ...], "test":[[user_id,item_id], ...] } user_id, item_id为整数，rating为浮点（1-5），无缺失值，无额外字段。 输出描述：单行JSON数组：["3.00", "3.00", "3.50"]。顺序与输入test完全一致，每个值为两位小数，含尾部为0，以字符串形式出书。 请用python实现代码，以ACM形式呈现

import sys
import json
import numpy as np
import pandas as pd

data = json.loads(sys.stdin.read())
train = pd.DataFrame(data['train'], columns=['user', 'item', 'rating'])
test = pd.DataFrame(data['test'], columns=['user', 'item'])

# 1. 构建(user, item)评分字典
ui_dict = {(u, i): r for u, i, r in train.values}

# 2. 用户均值
user_mean = train.groupby('user')['rating'].mean()

# 3. 全局均值
global_mean = train['rating'].mean()

res = []
for u, i in test.values:
    if (u, i) in ui_dict:
        pred = ui_dict[(u, i)]
    elif u in user_mean:
        pred = user_mean[u]
    else:
        pred = global_mean
    pred = min(5.0, max(1.0, pred))
    res.append(f"{round(pred + 1e-8, 2):.2f}")

print(json.dumps(res))