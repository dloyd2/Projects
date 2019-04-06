using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BlobBig : Enemy
{
    GameObject smallerBlob;
    
    public override void Awake()
    {
        base.Awake();
        startingHealth = 5;
        defense = 1;
        chanceToDrop = 40f;
        remainingHealth = startingHealth;
        smallerBlob = Resources.Load<GameObject>("Enemies/Blob");
        score = 5;
        SpawnManager.ChangeNumEnemies(1);
        type = EnemyType.BIG_BLOB;
    }
    // Start is called before the first frame update
    void Start()
    {
        
    }
    
    public override void Death()
    {
        GameObject spawn;
        isDead = true;
        spawn = Instantiate(smallerBlob) as GameObject;
        spawn.transform.position = transform.position;
        spawn.GetComponent<Rigidbody2D>().AddForce(new Vector2(-1*(float)Random.Range(40, 100), (float)Random.Range(100, 300)));
        spawn = Instantiate(smallerBlob) as GameObject;
        spawn.transform.position = transform.position;
        spawn.GetComponent<Rigidbody2D>().AddForce(new Vector2((float)Random.Range(40, 100), (float)Random.Range(100, 300)));
        SpawnManager.ChangeNumEnemies(-1);
        DropManager.Drop(gameObject, chanceToDrop);
        ScoreManager.ChangeScore(score);
        Destroy(gameObject, 0.5f);
    }
    
    // Update is called once per frame
    void Update()
    {
        CheckStatus();
    }
    
}
