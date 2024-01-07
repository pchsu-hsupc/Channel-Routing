import random
from collections import defaultdict


def RandomIdxs(n: int, sameRate: float = 1.):
    a = round(sameRate * n)
    idxs = list(range(a))
    for _ in range(n - a):
        idxs.append(random.randint(0, a))

    random.shuffle(idxs)
    return idxs


def MakeEdge(terminals: list[int], name: str, sameRate: float = 1.) -> list[str]:
    nZeros = terminals.count(0)
    idxs = RandomIdxs(nZeros + 1, sameRate)
    edges, idx, head, lastIdx = [], 0, 0, -1
    for i, t in enumerate(terminals):
        if t == 0:
            if idxs[idx] != lastIdx:
                lastIdx = idxs[idx]
                edges.append(f"{name}{lastIdx} {head} {i}")
            else:
                edges[-1] = " ".join(edges[-1].split()[:-1] + [str(i)])

            head = i
            idx += 1

    if idxs[idx] != lastIdx:
        lastIdx = idxs[idx]
        edges.append(f"{name}{lastIdx} {head} {i}")
    else:
        edges[-1] = " ".join(edges[-1].split()[:-1] + [str(i)])
        
    return edges
    

def MakeTerminalLists(nNetNumList: list[int], nBreak: int, nNet: int) -> tuple[list[int], list[int]]:
    netNumberMap = {
        i: (random.choice(nNetNumList) if i != 0 else nBreak) for i in range(nNet + 1)
    }

    terminals = []
    for net, number in netNumberMap.items():
        terminals.extend([net for _ in range(number)])

    random.shuffle(terminals)
    nTerminal = len(terminals)
    assert nTerminal % 2 == 0, "Number of terminals must be even."

    return terminals[:nTerminal // 2], terminals[nTerminal // 2:]


def IsFeasibleTerminals(upperTerminals: list[int], lowerTerminals: list[int]) -> bool:

    def IsHasTwoZero(terminals: list[int]) -> bool:
        for i in range(len(terminals) - 1):
            if terminals[i] == 0 and terminals[i + 1] == 0:
                return True
        return False
    
    def IsHasVerticalTerminal(upperTerminals: list[int], lowerTerminals: list[int]) -> bool:
        for u, l in zip(upperTerminals, lowerTerminals):
            if u == l:
                return True
        return False
    
    def IsCycleConflict(upperTerminals: list[int], lowerTerminals: list[int]) -> bool:

        def DFS(v: str, graph: dict[str, set[str]], t: str) -> bool:
            if v not in graph:
                return False

            visitMap = {k: False for k in graph}
            stack = [v]
            while stack:
                c = stack.pop()
                visitMap[c] = True
                for n in graph[c]:
                    if n == t: 
                        return True
                    if n in visitMap and not visitMap[n]:
                        stack.append(n)
            return False
                
        countMap = defaultdict(int)
        for u, l in zip(upperTerminals, lowerTerminals):
            countMap[u] += 1
            countMap[l] += 1

        headMap = defaultdict(int)
        graph   = defaultdict(set)
        for u, l in zip(upperTerminals, lowerTerminals):
            if u == 0 or l == 0:
                if u != 0: headMap[u] += 1
                if l != 0: headMap[l] += 1
                continue

            isUpperInit = headMap[u] == 0
            isLowerInit = headMap[l] == 0
            if not (isUpperInit or isLowerInit):
                graph[f"{u}_{headMap[u]}"].add(f"{l}_{headMap[l]}")

            headMap[u] += 1
            headMap[l] += 1
            isUpperAdd = headMap[u] < countMap[u]
            isLowerAdd = headMap[l] < countMap[l]
            if isLowerAdd:
                if isUpperAdd:
                    graph[f"{u}_{headMap[u]}"].add(f"{l}_{headMap[l]}")
                if not isUpperInit:
                    graph[f"{u}_{headMap[u] - 1}"].add(f"{l}_{headMap[l]}")

            if isUpperAdd:
                if not isLowerInit:
                    graph[f"{u}_{headMap[u]}"].add(f"{l}_{headMap[l] - 1}")
        
        for v in graph:
            if DFS(v, graph, v):
                return True
        
        return False
    

    if upperTerminals[0] == 0 or lowerTerminals[0] == 0 or upperTerminals[-1] == 0 or lowerTerminals[-1] == 0:
        return False
    if IsHasTwoZero(upperTerminals) or IsHasTwoZero(lowerTerminals):
        return False
    if IsHasVerticalTerminal(upperTerminals, lowerTerminals):
        return False
    if IsCycleConflict(upperTerminals, lowerTerminals):
        return False
    
    return True


def Main():
    
    SAVE_FILE      = "data/input9.in"
    N_NET          = 40
    N_BREAK        = 10
    N_NET_NUM_LIST = [2, 4, 6]
    SAME_EDGE_RATE = 0.3

    upperTerminals, lowerTerminals = MakeTerminalLists(N_NET_NUM_LIST, N_BREAK, N_NET)
    while (not IsFeasibleTerminals(upperTerminals, lowerTerminals)):
        upperTerminals, lowerTerminals = MakeTerminalLists(N_NET_NUM_LIST, N_BREAK, N_NET)

    print("")
    print("Upper terminals:")
    print(upperTerminals)
    print("Lower terminals:")
    print(lowerTerminals)

    upperEdges = MakeEdge(upperTerminals, "T", SAME_EDGE_RATE)
    lowerEdges = MakeEdge(lowerTerminals, "B", SAME_EDGE_RATE)
    print("")
    print(f"Upper Edges: (len = {len(upperTerminals)})")
    print(upperEdges)
    print(f"Lower Edges: (len = {len(lowerTerminals)})")
    print(lowerEdges)
    print("")

    with open(SAVE_FILE, "w") as f:
        for e in upperEdges:
            f.write(e + '\n')
        for e in lowerEdges:
            f.write(e + '\n')
        
        f.write(" ".join([str(i) for i in upperTerminals]) + '\n')
        f.write(" ".join([str(i) for i in lowerTerminals]))


if __name__ == "__main__":

    Main()