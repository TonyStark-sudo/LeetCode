#!/usr/bin/python
# -*- coding: utf-8 -*-

class Solution:
    def reverseWords(self, s):
        return ' '.join(reversed(s.split()))
    
if __name__ == '__main__':
     
    s = 'the sky is blue'
    # sol = Solution()
    # print(sol.reverseWords(s))
    print(Solution().reverseWords(s))