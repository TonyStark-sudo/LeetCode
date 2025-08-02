def seletSort(nums: list):
    n = len(nums)
    for i in range(0, n):
        min_index = i
        for j in range(i + 1, n):
            if (nums[j] < nums[min_index]):
                min_index = j
        # nums[i] = nums[min_index]
        nums[i], nums[min_index] = nums[min_index], nums[i]
        print(nums)

def selectSort2(nums: list):
    n = len(nums)
    for i in range(0, n - 1):
        for j in range(i + 1, n):
            if (nums[j] < nums[i]):
                nums[i], nums[j] = nums[j], nums[i]
        print(nums)

if __name__ == "__main__":
    nums = [10, 3, 1, 8, 4,  7, 6]
    print(nums)
    print("After Sort:")
    # seletSort(nums)
    selectSort2(nums)
    # print(1 in (1, 3, 4, 5))
    # print(nums)
