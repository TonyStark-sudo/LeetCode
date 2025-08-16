def radixSort(nums: list):
    n = len(nums)
    base = 1
    maxV = max(nums)
    while base < maxV:
        bucket = []
        for idx in range(10):
            bucket.append([])
        for num in nums:
            idx = num // base % 10
            bucket[idx].append(num)
        l = 0
        for idx in range(10):
            for v in bucket[idx]:
                nums[l] = v
                l += 1
        print(nums)
        base *= 10

if __name__ == "__main__":
    nums = [3121, 897, 3122, 3, 23, 5, 55, 7, 97, 123, 456, 1327]
    radixSort(nums)