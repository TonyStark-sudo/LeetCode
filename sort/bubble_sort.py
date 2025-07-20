def bubbleSort(nums: list):
    n = len(nums)
    for i in range(0, n - 1):
        for j in range(0, n - i - 1):
            if (nums[j] > nums[j + 1]):
                nums[j], nums[j + 1] = nums[j + 1], nums[j]
        print(nums)

if __name__ == "__main__":
    nums = [2, 3, 1, 8, 4, 6]
    print("Before Sorted: ", nums)
    bubbleSort(nums)
    print("After Sorted: ", nums)
        