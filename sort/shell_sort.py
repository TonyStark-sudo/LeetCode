
def shellSort(nums: list):
    n = len(nums)
    gap = n // 2
    while gap > 0:
        for i in range(gap, n):
            x = nums[i]
            j = i
            while j >= gap:
                if x < nums[j - gap]:
                    nums[j] = nums[j - gap]
                else:
                    break
                j -= gap
            nums[j] = x
        print(nums)
        gap = gap // 2
if __name__ == "__main__":
    nums = [10, 3, 1, 8, 4, 7, 6]
    shellSort(nums)
