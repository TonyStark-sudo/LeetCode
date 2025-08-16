def QuickSortPivot(nums: list, start: int, end: int):
    pivot = start
    j = start + 1
    for i in range(j, end + 1):
        if (nums[i] <= nums[pivot]):
            nums[i], nums[j] = nums[j], nums[i]
            j += 1
    nums[pivot], nums[j - 1] = nums[j - 1], nums[pivot]
    pivot = j - 1
    print(nums[pivot], nums[start:pivot], nums[pivot + 1:end + 1])
    return pivot

def QuickSort(nums: list, start: int, end: int):
    if start >= end:
        return 
    pivot = QuickSortPivot(nums, start, end)
    QuickSort(nums, start, pivot - 1)
    QuickSort(nums, pivot + 1, end)

if __name__ == "__main__":
    nums = [10, 3, 1, 8, 4,  7, 6]
    QuickSort(nums, 0, len(nums) - 1)
    print(nums)