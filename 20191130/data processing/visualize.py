import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.container import BarContainer
import io

def bar_caption(_bar: BarContainer) -> BarContainer:
    for rect in _bar:
        height = rect.get_height()
        plt.text(rect.get_x() + rect.get_width()/2.0, height, f'{height}', ha='center', va='bottom', size = 10)  # 'top', 'bottom', 'center', 'baseline', 'center_baseline'
    return _bar

res = open(r'20191130/data processing/test-result.txt', 'w', encoding='utf-8')

# 노드 50개 테스트 결과
data = pd.read_csv(r"20191130/basic GA/res/un50test.csv", header=0, index_col=0, encoding='utf-8')
buffer = io.StringIO()

res.write("un50test result\n")
res.write(f"{str(data.describe())}\n\n")

plt.figure(figsize=(12, 8))
plt.grid(alpha=0.3)
bar = plt.bar(data.index, data['cost'], align='center', alpha=0.8)
bar = bar_caption(bar)

plt.title("unweighted 50")
plt.ylim(data['cost'].min() // 10 * 10 - 10, data['cost'].max() // 10 * 10 + 10)
plt.yticks(rotation=0)

plt.savefig(r'20191130/data processing/un50test-graph.jpg')

# 노드 100개 테스트 결과
data = pd.read_csv(r"20191130/basic GA/res/un100test.csv", header=0, index_col=0, encoding='utf-8')
buffer = io.StringIO()

res.write("un100test result\n")
res.write(f"{str(data.describe())}\n\n")

plt.figure(figsize=(12, 8))
plt.grid(alpha=0.3)
bar = plt.bar(data.index, data['cost'], align='center', alpha=0.8)
bar = bar_caption(bar)

plt.title("unweighted 100")
plt.ylim(data['cost'].min() // 10 * 10 - 10, data['cost'].max() // 10 * 10 + 10)
plt.yticks(rotation=0)

plt.savefig(r'20191130/data processing/un100test-graph.jpg')

# 노드 500개 테스트 결과
data = pd.read_csv(r"20191130/basic GA/res/w500test.csv", header=0, index_col=0, encoding='utf-8')
buffer = io.StringIO()

res.write("w500test result\n")
res.write(f"{str(data.describe())}\n\n")

plt.figure(figsize=(12, 8))
plt.grid(alpha=0.3)
bar = plt.bar(data.index, data['cost'], align='center', alpha=0.8)
bar = bar_caption(bar)

plt.title("unweighted 50")
plt.ylim(data['cost'].min() // 10 * 10 - 10, data['cost'].max() // 10 * 10 + 10)
plt.yticks(rotation=0)

plt.savefig(r'20191130/data processing/w500test-graph.jpg')

res.close()