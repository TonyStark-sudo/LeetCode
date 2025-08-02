def insertSort(nums: list):
    n = len(nums)
    # 从第二个元素开始（索引1），因为第一个元素可以看作已排序
    for i in range(1, n):
        current = nums[i]  # 当前要插入的元素
        j = i - 1  # 已排序部分的最后一个位置
        
        # 在已排序部分中从右到左找插入位置
        while j >= 0 and nums[j] > current:
            nums[j + 1] = nums[j]  # 向右移动元素
            j -= 1
        
        # 插入当前元素到正确位置
        nums[j + 1] = current
        print(f"bound {i}: {nums}")

if __name__ == "__main__":
    nums = [10, 3, 1, 8, 4, 7, 6]
    print(f"Before Sorted: {nums}")
    insertSort(nums)
    print(f"After Sorted: {nums}")