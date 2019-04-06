using System.Collections;
using System.Collections.Generic;
using UnityEngine;
public enum EnemyType{BLOB = 0, BIG_BLOB, BAT, SKELETON, WOLFHEAD, NUMENEMIES};
//change max spawn value when enum length changes
public class Auxillary : MonoBehaviour
{
    static public Dictionary<EnemyType, int> enemyValues = new Dictionary<EnemyType, int>();
    static public Dictionary<EnemyType, GameObject> enemies = new Dictionary<EnemyType, GameObject>();
    
    static public string FindType(EnemyType input)
    {
        switch(input)
        {
            case EnemyType.BLOB: return "blob";
            case EnemyType.BIG_BLOB: return "big_blob";
            case EnemyType.SKELETON: return "skeleton";
            case EnemyType.BAT: return "bat";
            case EnemyType.WOLFHEAD: return "wolf_head";
            default:
                return "not found";
        }
    }
    void Awake()
    {
        if(!enemyValues.ContainsKey(EnemyType.BLOB))
            enemyValues.Add(EnemyType.BLOB, 1);
        if(!enemies.ContainsKey(EnemyType.BLOB))
            enemies.Add(EnemyType.BLOB, Resources.Load("Enemies/Blob") as GameObject);
        if(!enemyValues.ContainsKey(EnemyType.BIG_BLOB))
            enemyValues.Add(EnemyType.BIG_BLOB, 3);
        if(!enemies.ContainsKey(EnemyType.BIG_BLOB))
            enemies.Add(EnemyType.BIG_BLOB, Resources.Load("Enemies/Blob_Big") as GameObject);
        if(!enemyValues.ContainsKey(EnemyType.SKELETON))
            enemyValues.Add(EnemyType.SKELETON, 5);
        if(!enemies.ContainsKey(EnemyType.SKELETON))
            enemies.Add(EnemyType.SKELETON, Resources.Load("Enemies/Skeleton") as GameObject);
        if(!enemyValues.ContainsKey(EnemyType.BAT))
            enemyValues.Add(EnemyType.BAT, 4);
        if(!enemies.ContainsKey(EnemyType.BAT))
            enemies.Add(EnemyType.BAT, Resources.Load("Enemies/Bat") as GameObject);
        if(!enemyValues.ContainsKey(EnemyType.WOLFHEAD))
            enemyValues.Add(EnemyType.WOLFHEAD, 8);
        if(!enemies.ContainsKey(EnemyType.WOLFHEAD))
            enemies.Add(EnemyType.WOLFHEAD, Resources.Load("Enemies/WolfHead") as GameObject);
    }

    // Update is called once per frame
    void Update()
    {
        
    }
}
