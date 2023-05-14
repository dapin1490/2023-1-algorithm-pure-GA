import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.container import BarContainer

def bar_caption(_bar: BarContainer) -> BarContainer:
    for rect in _bar:
        height = rect.get_height()
        plt.text(rect.get_x() + rect.get_width()/2.0, height, f'{height}', ha='center', va='bottom', size = 10)  # 'top', 'bottom', 'center', 'baseline', 'center_baseline'
    return _bar

def save_stat(data_route: str, file, fig_route, title: str, fgsize: tuple=(12, 8)):
    data = pd.read_csv(data_route, header=0, index_col=0, encoding='utf-8')
    data.sort_values(by='cost', inplace=True)
    data.reset_index(inplace=True, drop=True)

    file.write(f"{title}\n")
    file.write(f"{str(data.describe())}\n\n")

    plt.figure(figsize=fgsize)
    plt.grid(alpha=0.3)
    bar = plt.bar(data.index, data['cost'], align='center', alpha=0.8)
    bar = bar_caption(bar)

    plt.plot(data.index, [data['cost'].mean()]*len(data.index), 'm--', alpha=0.5)
    plt.legend([f'mean: {data["cost"].mean():.2f}'], loc='upper left')

    plt.title(f"{title}(std: {data['cost'].std():.2f})")
    plt.ylim(data['cost'].min() // 10 * 10 - 10, data['cost'].max() // 10 * 10 + 10)
    plt.yticks(rotation=0)

    plt.savefig(fig_route)
    return

res = open(r'20191130/data processing/test-result.txt', 'w', encoding='utf-8')

# 노드 50개 테스트 결과
save_stat(r"20191130/basic GA/res/un50test.csv", res, r'20191130/data processing/un50test-graph.jpg', "unweighted 50")

# 노드 100개 테스트 결과
save_stat(r"20191130/basic GA/res/un100test.csv", res, r'20191130/data processing/un100test-graph.jpg', "unweighted 100")

# 노드 500개 테스트 결과
save_stat(r"20191130/basic GA/res/w500test.csv", res, r'20191130/data processing/w500test-graph.jpg', "weighted 500")

res.close()