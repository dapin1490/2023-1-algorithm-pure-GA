import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv(r'20191130/data processing/GA run/un100describe.csv', header=0, index_col=0, encoding='utf-8')

for col in data.columns:
    print(col)

    plt.figure(figsize=(12, 8))  # 그래프 크기
    plt.grid(alpha=0.3)  # 그리드 투명도

    plt.plot(data.index, data[col], marker='.', alpha=0.8)

    plt.title(f"un100 {col}")  # 그래프 제목에 괄호 치고 분산 표기
    # plt.ylim(data[col].min() // 10 * 10 - 10, data[col].max() // 10 * 10 + 10)  # y 값 범위 제한

    plt.xlabel('generation')
    plt.ylabel(col, rotation='horizontal')

    plt.savefig(rf'20191130/data processing/GA run/images/un100{col}.jpg')
    plt.clf()
