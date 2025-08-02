def countingSort(nums: list):
    maxValue = max(nums)
    counter = [0] * (maxValue + 1)
    for num in nums:
        counter[num] += 1
    nums.clear()
    nums = [] # 定义局部变量会在函数结束时摧毁，不会传递到入参的nums
    for i in range(0, len(counter)):
        while (counter[i]):
            nums.append(i)
            counter[i] -= 1
    
if __name__ == "__main__":
    nums = [10, 3, 1, 8, 4, 7, 6]
    print(f"Before Sorted: {nums}")
    countingSort(nums)
    print(f"After Sorted: {nums}")