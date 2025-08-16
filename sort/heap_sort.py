# def maxHeapify(heap, start, end):
#     parent = start
#     son = start * 2
#     while son <= end:
#         if son + 1 <= end and heap[son + 1] > heap[son]:
#             son += 1
#         if heap[son] > heap[parent]:
#             heap[parent], heap[son] = heap[son], heap[parent]
#             parent, son = son, son * 2
#         else:
#             break

# def HeapSort(nums: list):

def maxHeapify(heap, start, end):
    """向下调整，维护最大堆性质"""
    parent = start
    son = start * 2  # 左子节点
    
    while son <= end:
        # 找到较大的子节点
        if son + 1 <= end and heap[son + 1] > heap[son]:
            son += 1  # 选择右子节点
        
        # 如果子节点比父节点大，交换
        if heap[son] > heap[parent]:
            heap[parent], heap[son] = heap[son], heap[parent]
            parent, son = son, son * 2  # 继续向下调整
        else:
            break  # 已经满足堆性质，退出

def HeapSort(nums: list):
    """堆排序主函数"""
    if len(nums) <= 1:
        return nums
    
    # 注意：这里使用1-based索引，所以在开头插入一个占位元素
    heap = [0] + nums  # 为了方便计算，索引从1开始
    n = len(nums)
    
    # 第一步：建立最大堆（从最后一个非叶子节点开始）
    for i in range(n // 2, 0, -1):
        maxHeapify(heap, i, n)
    
    # 第二步：依次取出堆顶元素（最大值）
    for i in range(n, 1, -1):
        # 将堆顶（最大值）与末尾元素交换
        heap[1], heap[i] = heap[i], heap[1]
        # 对剩余元素重新调整为最大堆
        maxHeapify(heap, 1, i - 1)
    
    # 返回排序结果（去掉占位元素）
    return heap[1:]

# 测试函数
def print_heap_process(nums):
    """显示堆排序过程"""
    print(f"原数组: {nums}")
    
    if len(nums) <= 1:
        return nums
    
    heap = [0] + nums  # 1-based索引
    n = len(nums)
    
    print("\n=== 建堆过程 ===")
    # 建堆
    for i in range(n // 2, 0, -1):
        print(f"调整节点 {i}:")
        maxHeapify(heap, i, n)
        print(f"  结果: {heap[1:]}")
    
    print(f"\n初始最大堆: {heap[1:]}")
    
    print("\n=== 排序过程 ===")
    # 排序
    for i in range(n, 1, -1):
        # 交换堆顶与末尾
        heap[1], heap[i] = heap[i], heap[1]
        print(f"取出最大值 {heap[i]}, 剩余: {heap[1:i]}")
        
        # 重新调整堆
        maxHeapify(heap, 1, i - 1)
        print(f"调整后的堆: {heap[1:i]}")
        print("---")
    
    return heap[1:]

if __name__ == "__main__":
    nums = [4, 6, 8, 5, 9, 1, 3]
    result = print_heap_process(nums)
    print(f"\n最终排序结果: {result}")